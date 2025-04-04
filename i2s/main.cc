#include "drivers/codec_PCM3168.hh"
#include "drivers/console.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
#include "drivers/grf_iofunc.hh"
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

struct Params {
	unsigned hit_ctr = 0;
	unsigned hit_rate = 12'000;
	std::array<bool, 4> knob_changed{true, true, true, true};
	std::array<float, 4> knobs{0.5f, 0.5f, 0.5f, 0.5f};
};

// Used to allow console commands access to commands
Params *g_params = nullptr;

CONSOLE_COMMAND_DEF(k, "Set a knob percent", CONSOLE_INT_ARG_DEF(knob, "0 to 3"), CONSOLE_INT_ARG_DEF(val, "0 to 100"));
static void k_command_handler(const k_args_t *args) {
	if (g_params) {
		g_params->knobs[args->knob] = std::clamp((float)args->val / 100.f, 0.f, 1.f);
		printf("Set knob %ld to %f\n", args->knob, g_params->knobs[args->knob]);
		g_params->knob_changed[args->knob] = true;
	}
}

CONSOLE_COMMAND_DEF(rate, "Period of hits (default = 12)", CONSOLE_INT_ARG_DEF(rate, "1 to 100"));
static void rate_command_handler(const rate_args_t *args) {
	if (g_params) {
		g_params->hit_rate = args->rate * 1000;
	}
}

void delay_us(unsigned us) {
	using namespace mdrivlib;

	// toggles at 8MHz, so to delay 1us -> 8 toggles
	for (unsigned i = 0; i < 8u * us; i++) {
		GPIO0->high(Gpio::Port::C, 6);
		GPIO0->low(Gpio::Port::C, 6);
	}
}

void init_i2c();
void init_i2s1_clocks();
void init_i2s1_pins();
void init_codec();

int main() {
	using namespace mdrivlib;

	printf("\nStarting I2S example\n");
	console_command_register(k);
	console_command_register(rate);

	mdrivlib::RockchipPeriph::Cru::Apll::fbdiv::write(0x44);
	mdrivlib::RockchipPeriph::Cru::Apll::postdiv1::write(0x1);
	mdrivlib::RockchipPeriph::Cru::Apll::bypass::clear();

	// Set up GPIO0_C5 as output (used for delay and for timing)
	GPIO0->dir_output(Gpio::Port::C, 5);
	GPIO0->low(Gpio::Port::C, 5);

	GPIO0->dir_output(Gpio::Port::C, 6);

	// Pin reset_pin{GPIO::GPIO0, PinNum::B6, PinMode::Output};
	// reset_pin.low();

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

	I2S1->tx8_parallel_mode();
	I2S1->rx_stereo_mode();
	I2S1->master_tx();

	// Setup interrupt
	Params params;
	g_params = &params;

	constexpr uint32_t BlockSize = 8;
	constexpr float kOutScaling = static_cast<float>(0x7F'FFFF);

	std::array<MetaModule::DjembeCore, 2> djs;
	// MetaModule::DjembeCore dj;

	for (auto i = 0u; auto &dj : djs) {
		dj.set_samplerate(48000);
		dj.set_param(0, float(i) / float(djs.size()) + 0.1f);
		dj.set_param(1, 0.5f);
		dj.set_param(2, 0.5f);
		dj.set_param(3, 0.5f);
		dj.set_input(0, 0);
		dj.set_input(1, 0);
		dj.set_input(2, 0);
		dj.set_input(3, 0);
		dj.set_input(4, 0);
		i++;
	}

	// with one or two writes to TXDR: blocks of 8 take 2.78us = 1.6% load
	// Reading RXDR twice, and writing TXDR twice; 7.78us = 4.65% load

	// with USE_I2S_RX:
	// # djembes  load
	// 2  5.87%    2.9% ea
	// 4  8.80%    2.2% ea
	// 8  14.62%
	// 16 26.4%
	// 20 40%   2.0% ea
	// 24  two 94us pulses in a row

	// without RX:
	// 20 37%   1.85% each
	//
	// without RX, but with two loops
	// 2   3.2%
	// 8  11.9%
	// 24  53%
	// 32  84%   2.625% each

	static std::array<int32_t, BlockSize * 2> outbuf;

	InterruptManager::register_and_start_isr(IRQ::I2S1_8CH_IRQ, 0, 0, [&djs, &params] {
		GPIO0->high(Gpio::Port::C, 5);

		for (auto i = 0u; i < BlockSize; i++) {
			I2S1->TXDR = outbuf[i * 2];
			I2S1->TXDR = outbuf[i * 2 + 1];
		}

		for (auto i = 0u; i < BlockSize; i++) {
			params.hit_ctr++;

			for (auto knob_id = 0; auto &changed : params.knob_changed) {
				if (changed) {

					for (auto &dj : djs) {
						dj.set_param(knob_id, params.knobs[knob_id]);
						changed = false;
					}

					knob_id++;
				}
			}

			float out = 0;
			for (auto dj_idx = 0u; auto &dj : djs) {
				unsigned hit_time = dj_idx * params.hit_rate / djs.size();
				// unsigned hit_time = params.hit_rate;
				dj.set_input(4, params.hit_ctr % hit_time == 0 ? 1 : 0);

				dj.update();

				out += dj.get_output(0); // / djs.size();

				dj_idx++;
			}

			auto v = static_cast<int32_t>(out * kOutScaling);

			// 			auto inL = I2S1->RXDR;
			// 			auto inR = I2S1->RXDR;
			// 			I2S1->TXDR = inL + inR;
			outbuf[i * 2] = v;
			outbuf[i * 2 + 1] = v;
		}
		GPIO0->low(Gpio::Port::C, 5);
	});

	I2S1->enable_TX_ISR_with_block_size(BlockSize);

	// Enable IRQs
	printf("Enable IRQ\n");
	mdrivlib::IRQ_init();
	enable_irq();

	printf("Enable I2S pins\n");
	init_i2s1_pins();

	init_codec();

	printf("Start TX\n");
	I2S1->start_txrx();

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

	CruGate::hclk_gic_audio_en::write(CruGate::clock_enable);

	CruGate::hclk_i2s1_8ch_en::write(CruGate::clock_enable);

	Cru::mresetn_i2s1_8ch_tx::set();
	Cru::mresetn_i2s1_8ch_rx::set();
	delay_us(10);
	Cru::mresetn_i2s1_8ch_tx::clear();
	Cru::mresetn_i2s1_8ch_rx::clear();
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
	CruClksel::i2s1_8ch_rx_src_div::write(97);

	// Select the I2S clock source to be gpll
	printf("Set I2S clock source\n");
	CruClksel::i2s1_8ch_tx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);
	CruClksel::i2s1_8ch_rx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);

	// Select the MCLK source clock to the integral divided clock
	CruClksel::mclk_i2s1_8ch_tx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	CruClksel::mclk_i2s1_8ch_rx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);

	CruGate::mclk_i2s1_8ch_tx_en::write(CruGate::clock_enable);

	CruGate::i2s1_mclkout_tx_en::write(CruGate::clock_enable);
}

void init_i2s1_pins() {
	using namespace mdrivlib;
	using namespace mdrivlib::RockchipPeriph;

	SysGrf::i2s1_mclk_sel::write(SysGrf::con1_i2s1_mclk_sel::i2s1_mclk_tx);
	SysGrf::i2s1_mclk_tx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_cru);
	// SysGrf::i2s1_mclk_rx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_ext_chip);

	GrfIofunc::i2s1_iomux_sel::write(GrfIofunc::choice_iomux3::m1);

	SYS_GPIO_IOMUX->gpio3_c_h.write(GPIO3C_IOMUX_H_SEL_7::I2S1_SCLKTXM1);
	SYS_GPIO_IOMUX->gpio3_c_h.write(GPIO3C_IOMUX_H_SEL_6::I2S1_MCLKM1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_1::I2S1_SDO0M1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_2::I2S1_SDI0M1);
	SYS_GPIO_IOMUX->gpio3_d_l.write(GPIO3D_IOMUX_L_SEL_0::I2S1_LRCKTXM1);
	SYS_GPIO_IOMUX->gpio4_a_h.write(GPIO4A_IOMUX_H_SEL_7::I2S1_LRCKRXM1);
}

void init_codec() {
	using namespace mdrivlib;
	using namespace mdrivlib::RockchipPeriph;

	// Pins 27 (I2C2_SDA_M1) and 28 (I2C2_SCL_M1)
	auto i2cconf = I2CConfig{
		.I2C_periph_num = 2,
		.SCL = {.gpio = GPIO::GPIO4, .pin = PinNum::B5, .af = (uint8_t)GPIO4B_IOMUX_H_SEL_5::I2C2_SCLM1},
		.SDA = {.gpio = GPIO::GPIO4, .pin = PinNum::B4, .af = (uint8_t)GPIO4B_IOMUX_H_SEL_4::I2C2_SDAM1},
		.timing = {100'000},
	};

	// TODO: should this happen in pin.cc?
	GrfIofunc::i2c2_iomux_sel::write(GrfIofunc::choice_iomux2::m1);

	auto i2c = I2CPeriph{i2cconf};

	auto sai = SaiConfig{.sai_periphnum = 2,
						 .tx_block_num = 0,
						 .rx_block_num = 0,
						 .mode = SaiConfig::SAIRxTxMode::TXMaster,
						 .dma_init_tx = {},
						 .dma_init_rx = {},
						 .datasize = 24,
						 .framesize = 32,
						 .samplerate = 48000,
						 // TODO: pins
						 .reset_pin = PinDef{GPIO::GPIO0, PinNum::B6},
						 .bus_address = 1,
						 .num_tdm_ins = 2,
						 .num_tdm_outs = 2};

	printf("Create codec\n");
	CodecPCM3168 codec{i2c, sai};
	codec.init();
}
