#include "drivers/console.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/gic.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
#include "drivers/i2s.hh"
#include "drivers/interrupt.hh"
#include "drivers/irq_init.hh"
#include "drivers/irqs.hh"
#include "drivers/pwm.hh"
#include <cstdio>

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(pin, "pin", CONSOLE_INT_ARG_DEF(onoff, "1=on 0=off"));
static void pin_command_handler(const pin_args_t *args) {
	if (args->onoff)
		HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
	else
		HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);
}

int main() {
	printf("\nStarting I2S example\n");
	console_command_register(pin);

	using namespace RockchipPeriph;
	using namespace mdrivlib::RockchipPeriph;
	using namespace mdrivlib;

	// Pins:
	// (22) GPIO3_C6: I2S1_MCLK_M1
	// (12) GPIO3_C7: I2S1_SCLK_TX_M1
	// (35) GPIO3_D0: I2S1_LRCK_TX_M1
	// (40) GPIO3_D1: I2S1_SDO0_M1
	// (38) GPIO3_D2: I2S1_SDI0_M1

	// Clocks:
	// Main clock (gpll -> hclkc_gic_audio)
	Cru::hclk_gic_audio_sel::write(Cru::hclk_gic_audio_clock_mux::clk_gpll_div_100m);
	// wr 0xfdd20128 0x0c000400

	Cru::hclk_gic_audio_en::write(Cru::cru_clock_enable);
	// wr 0xfdd20314 0x00020000

	Cru::hclk_i2s1_8ch_en::write(Cru::cru_clock_enable);
	// wr 0xfdd20314 0x08000000

	Cru::mresetn_i2s1_8ch_tx::set();
	// wr 0xfdd20414 0x00040004
	Cru::mresetn_i2s1_8ch_rx::set();
	// wr 0xfdd20414 0x00080008
	// delay 10us
	for (int i = 0; i < 80; i++) {
		HW::GPIO0->high(Gpio::Port::C, 5);
		HW::GPIO0->low(Gpio::Port::C, 5);
	}
	Cru::mresetn_i2s1_8ch_tx::clear();
	// wr 0xfdd20414 0x00040000
	Cru::mresetn_i2s1_8ch_rx::clear();
	// wr 0xfdd20414 0x00080000
	// delay 10us
	for (int i = 0; i < 80; i++) {
		HW::GPIO0->high(Gpio::Port::C, 5);
		HW::GPIO0->low(Gpio::Port::C, 5);
	}

	// Connect MCLKOUT to I2S1 TX mclk
	printf("Connect MCLKOUT\n\r");
	Cru::i2s1_mclkout_tx_sel::write(Cru::i2s_mclkout_sel::mclk_i2s_8ch);
	// wr 0xfdd2013c 0x80000000

	Cru::i2s1_mclkout_rx_sel::write(Cru::i2s_mclkout_sel::xin_osc0_half);
	// wr 0xfdd20144 0x80000000

	// Set the clock divider for cpll
	// TODO: need to calculate this so we hit 48kHz * 256 = 12.288MHz
	// Estimate cPLL is at 50MHz to start??? /4 is close
	// We will probably need to use the fractional divider to get this more exact
	Cru::i2s1_8ch_tx_src_div::write(0x13);
	// wr 0xfdd2013c 0x007f0013

	Cru::i2s1_8ch_rx_src_div::write(0x13);
	// wr 0xfdd20144 0x007f0013

	// Select the I2S clock source to be gpll
	// printf("Set I2S clock source\n\r");
	Cru::i2s1_8ch_tx_src_sel::write(Cru::clk_i2s_8ch_src_sel::clk_cpll_mux);
	// wr 0xfdd2013c 0x03000000

	Cru::i2s1_8ch_rx_src_sel::write(Cru::clk_i2s_8ch_src_sel::clk_cpll_mux);
	// wr 0xfdd20144 0x03000000

	// Select the MCLK source clock to the integral divided clock
	Cru::mclk_i2s1_8ch_tx_sel::write(Cru::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	// wr 0xfdd2013c 0x0c000000

	Cru::mclk_i2s1_8ch_rx_sel::write(Cru::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	// wr 0xfdd20144 0x0c000000

	Cru::mclk_i2s1_8ch_tx_en::write(Cru::cru_clock_enable);
	// wr 0xfdd20318 0x04000000

	Cru::i2s1_mclkout_tx_en::write(Cru::cru_clock_enable);
	// wr 0xfdd20318 0x08000000

	// Setup I2S
	/*
wr 0xfe410000 0x7200000f
wr 0xfe410004 0x01c8000f
wr 0xfe410008 0x00001f1f
wr 0xfe410010 0x001f0000
wr 0xfe410014 0x01f00000
wr 0xfe410030 0x00003eff
wr 0xfe410034 0x00003eff
wr 0xfe410038 0x00000707
	*/

	HW::I2S1->XFER = 0;
	HW::I2S1->CLR = 1;
	while (HW::I2S1->CLR != 0)
		;
	HW::I2S1->enable_DMA();
	HW::I2S1->tdm_tx8_mode();
	HW::I2S1->tdm_rx6_mode();
	HW::I2S1->master_tx();

	printf("Select TX to MCLK pin\n");
	GRF_SOC::i2s1_mclk_sel::write(GRF_SOC::con1_i2s1_mclk_sel::i2s1_mclk_tx);
	GRF_SOC::i2s1_mclk_tx_oe::write(GRF_SOC::con2_i2s1_mclk_oe::from_cru);
	GRF_SOC::i2s1_mclk_rx_oe::write(GRF_SOC::con2_i2s1_mclk_oe::from_ext_chip);

	// Setup DMA

	// Setup interrupt
	mdrivlib::InterruptManager::register_and_start_isr(IRQ::I2S1_8CH_IRQ, 0, 0, [] { printf("I2S1 IRQ\n"); });

	// Enable IRQs
	printf("Enable IRQ\n");
	mdrivlib::IRQ_init();
	enable_irq();

	printf("Setting pin mux\n");
	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_7::I2S1_SCLKTXM1);
	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_6::I2S1_MCLKM1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_1::I2S1_SDO0M1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_2::I2S1_SDI0M1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_0::I2S1_LRCKTXM1);

	printf("Enable XFER\n");
	HW::I2S1->XFER = 0b11;

	Console::init();

	while (true) {
		Console::process();

		asm("nop");
	}
}
