#include "drivers/console.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
#include "drivers/i2c.hh"
#include "drivers/i2s.hh"
#include "drivers/interrupt.hh"
#include "drivers/irq_init.hh"
#include "drivers/irqs.hh"
#include "drivers/pin.hh"
#include "drivers/pmu.hh"
#include "drivers/pwm.hh"
#include <cstdio>

#include "../djembe/djembe.hh"

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(pin, "pin", CONSOLE_INT_ARG_DEF(onoff, "1=on 0=off"));
static void pin_command_handler(const pin_args_t *args) {
	using namespace mdrivlib;

	if (args->onoff)
		GPIO0->high(Gpio::Port::C, 5);
	else
		GPIO0->low(Gpio::Port::C, 5);
}

CONSOLE_COMMAND_DEF(tx,
					"tx",
					CONSOLE_INT_ARG_DEF(data, "data to send"),
					CONSOLE_INT_ARG_DEF(num, "number of times to send"));
static void tx_command_handler(const tx_args_t *args) {
	using namespace mdrivlib;

	for (auto i = 0; i < args->num; i++)
		I2S1->TXDR = args->data;
}

void delay_us(unsigned us) {
	using namespace mdrivlib;

	// toggles at 8MHz, so to delay 1us -> 8 toggles
	for (unsigned i = 0; i < 8u * us; i++) {
		GPIO0->high(Gpio::Port::C, 5);
		GPIO0->low(Gpio::Port::C, 5);
	}
}

void init_i2c();
void init_i2s1_clocks();
void init_i2s1_pins();

int main() {
	using namespace mdrivlib;

	printf("\nStarting I2S example\n");
	console_command_register(pin);
	console_command_register(tx);

	// Set up GPIO0_C5 as output (used for delay and for timing)
	GPIO0->dir_output(Gpio::Port::C, 5);
	GPIO0->high(Gpio::Port::C, 5);

	Pin reset_pin{GPIO::GPIO0, PinNum::B6, PinMode::Output};
	reset_pin.low();

	// Connect these Pins:
	// (22) GPIO3_C6: I2S1_MCLK_M1
	// (12) GPIO3_C7: I2S1_SCLK_TX_M1
	// (35) GPIO3_D0: I2S1_LRCK_TX_M1
	// (40) GPIO3_D1: I2S1_SDO0_M1
	// (38) GPIO3_D2: I2S1_SDI0_M1
	//  (3) GPIO0_B6: CODEC_RESET

	init_i2s1_clocks();

	// Setup I2S
	I2S1->reset();

	// HW::I2S1->enable_DMA();
	I2S1->tx8_parallel_mode();
	// HW::I2S1->tdm_rx6_mode();
	I2S1->master_tx();

	// Setup interrupt
	constexpr uint32_t BlockSize = 8;
	MetaModule::DjembeCore dj;
	unsigned hit_ctr = 0;
	constexpr float kOutScaling = static_cast<float>(0x7F'FFFF);

	InterruptManager::register_and_start_isr(IRQ::I2S1_8CH_IRQ, 0, 0, [&hit_ctr, &dj] {
		I2S1->clear_tx_underrun();

		GPIO0->high(Gpio::Port::C, 5);
		for (auto i = 0u; i < BlockSize; i++) {
			hit_ctr++;
			dj.set_input(4, hit_ctr % 12'000 == 0 ? 1 : 0);
			dj.update();
			float out = dj.get_output(0);
			auto v = static_cast<int32_t>(out * kOutScaling);

			// L and R: same signal
			I2S1->TXDR = v;
			I2S1->TXDR = v;
		}
		GPIO0->low(Gpio::Port::C, 5);
	});

	I2S1->enable_TX_ISR_with_block_size(BlockSize);

	// Enable IRQs
	printf("Enable IRQ\n");
	mdrivlib::IRQ_init();
	enable_irq();

	init_i2s1_pins();

	reset_pin.high();
	delay_us(313);

	// TODO I2c config

	using namespace mdrivlib::RockchipPeriph;

	// Pins 19 (I2C4_SDA) and 23 (I2C4_SCL)
	auto i2cconf = I2CConfig{
		.I2C_periph_num = 4,
		.SCL = {.gpio = GPIO::GPIO4, .pin = PinNum::B3, .af = (uint8_t)GPIO4B_IOMUX_L_SEL_3::I2C4_SCLM0},
		.SDA = {.gpio = GPIO::GPIO4, .pin = PinNum::B2, .af = (uint8_t)GPIO4B_IOMUX_L_SEL_2::I2C4_SDAM0},
		.timing = {100'000},
	};

	auto i2c = I2CPeriph{i2cconf};
	uint8_t data[4] = {0xAA, 0xF0, 0xFF, 0x55};
	i2c.write(0x40, data, 4);
	///////////

	I2S1->start_tx();

	Console::init();

	while (true) {
		Console::process();

		asm("nop");
	}
}

void init_i2s1_clocks() {
	using namespace mdrivlib;
	using namespace mdrivlib::RockchipPeriph;

	// Clocks:

	// Main clock (gpll -> hclkc_gic_audio)
	CruClksel::hclk_gic_audio_sel::write(CruClksel::hclk_gic_audio_clock_mux::clk_gpll_div_150m);

	CruGate::hclk_gic_audio_en::write(CruGate::cru_clock_enable);

	CruGate::hclk_i2s1_8ch_en::write(CruGate::cru_clock_enable);

	Cru::mresetn_i2s1_8ch_tx::set();
	// Cru::mresetn_i2s1_8ch_rx::set();
	delay_us(10);
	Cru::mresetn_i2s1_8ch_tx::clear();
	// Cru::mresetn_i2s1_8ch_rx::clear();
	delay_us(10);

	// Connect MCLKOUT to I2S1 TX mclk
	printf("Connect MCLKOUT\n\r");
	CruClksel::i2s1_mclkout_tx_sel::write(CruClksel::i2s_mclkout_sel::mclk_i2s_8ch);

	// CruClksel::i2s1_mclkout_rx_sel::write(CruClksel::i2s_mclkout_sel::xin_osc0_half);

	// Set the clock divider for gpll
	// We will need to use the fractional divider to get this more exact
	// GPLL is 1200MHz, divide by 98 = 12.245MHz
	//
	// in parallel mode with CLKDIV 0x0303:
	//  97 (0x61) means /98 => MCLK 12.245Hz, SCLK = 3.061MHz, LRCLK = 47.831kHz
	//  ratios are 256:4:1
	CruClksel::i2s1_8ch_tx_src_div::write(97);

	// CruClksel::i2s1_8ch_rx_src_div::write(0x8);

	// Select the I2S clock source to be gpll
	printf("Set I2S clock source\n");
	CruClksel::i2s1_8ch_tx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);

	// CruClksel::i2s1_8ch_rx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);

	// Select the MCLK source clock to the integral divided clock
	CruClksel::mclk_i2s1_8ch_tx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);

	// CruClksel::mclk_i2s1_8ch_rx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);

	CruGate::mclk_i2s1_8ch_tx_en::write(CruGate::cru_clock_enable);

	CruGate::i2s1_mclkout_tx_en::write(CruGate::cru_clock_enable);
}

void init_i2s1_pins() {
	using namespace mdrivlib;
	using namespace mdrivlib::RockchipPeriph;

	SysGrf::i2s1_mclk_sel::write(SysGrf::con1_i2s1_mclk_sel::i2s1_mclk_tx);
	SysGrf::i2s1_mclk_tx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_cru);
	// SysGrf::i2s1_mclk_rx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_ext_chip);

	GrfIofunc::i2s1_iomux_sel_m1::write(GrfIofunc::choice_iomux3::m1);

	SYS_GPIO_IOMUX->gpio3_c_h.write(GPIO3C_IOMUX_H_SEL_7::I2S1_SCLKTXM1);
	SYS_GPIO_IOMUX->gpio3_c_h.write(GPIO3C_IOMUX_H_SEL_6::I2S1_MCLKM1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_1::I2S1_SDO0M1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_2::I2S1_SDI0M1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_0::I2S1_LRCKTXM1);
	SYS_GPIO_IOMUX->gpio4_a_h.write(GPIO4A_IOMUX_H_SEL_7::I2S1_LRCKRXM1);
}

void init_i2c() {
	using namespace mdrivlib;
	using namespace mdrivlib::RockchipPeriph;
}
