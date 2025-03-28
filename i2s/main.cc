#include "drivers/console.hh"

#include "drivers/cru_gate.hh"

#include "drivers/cru_clksel.hh"
#include "drivers/cru_reset.hh"
#include "drivers/gic.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
#include "drivers/i2s.hh"
#include "drivers/interrupt.hh"
#include "drivers/irq_init.hh"
#include "drivers/irqs.hh"
#include "drivers/pmu.hh"
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

CONSOLE_COMMAND_DEF(tx,
					"tx",
					CONSOLE_INT_ARG_DEF(data, "data to send"),
					CONSOLE_INT_ARG_DEF(num, "number of times to send"));
static void tx_command_handler(const tx_args_t *args) {
	for (auto i = 0; i < args->num; i++)
		HW::I2S1->TXDR = args->data;
}

void delay_us(uint32_t us) {
	// 10us -> 80
	// 1us -> 8
	for (unsigned i = 0; i < 8u * us; i++) {
		HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
		HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);
	}
}

int main() {
	printf("\nStarting I2S example\n");
	console_command_register(pin);
	console_command_register(tx);

	using namespace RockchipPeriph;
	using namespace mdrivlib::RockchipPeriph;
	using namespace mdrivlib;

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_output(Gpio::Port::C, 5);
	HW::GPIO0->high(Gpio::Port::C, 5);

	// Pins:
	// (22) GPIO3_C6: I2S1_MCLK_M1
	// (12) GPIO3_C7: I2S1_SCLK_TX_M1
	// (35) GPIO3_D0: I2S1_LRCK_TX_M1
	// (40) GPIO3_D1: I2S1_SDO0_M1
	// (38) GPIO3_D2: I2S1_SDI0_M1

	// Clocks:

	// alive clock
	// Pmu::alive_osc_ena::set();
	// Pmu::alive_osc_ena_sft::set();

	// Main clock (gpll -> hclkc_gic_audio)
	CruClksel::hclk_gic_audio_sel::write(CruClksel::hclk_gic_audio_clock_mux::clk_gpll_div_150m);
	// wr 0xfdd20128 0x0c000400
	// default is 0x0000 (gpll_150m)

	CruGate::hclk_gic_audio_en::write(CruGate::cru_clock_enable);
	// wr 0xfdd20314 0x00020000

	CruGate::hclk_i2s1_8ch_en::write(CruGate::cru_clock_enable);
	// wr 0xfdd20314 0x08000000

	Cru::mresetn_i2s1_8ch_tx::set();
	// Cru::mresetn_i2s1_8ch_rx::set();
	// wr 0xfdd20414 0x00040004
	// wr 0xfdd20414 0x00080008
	delay_us(10);
	Cru::mresetn_i2s1_8ch_tx::clear();
	// Cru::mresetn_i2s1_8ch_rx::clear();
	// wr 0xfdd20414 0x00040000
	// wr 0xfdd20414 0x00080000
	delay_us(10);

	// Connect MCLKOUT to I2S1 TX mclk
	printf("Connect MCLKOUT\n\r");
	CruClksel::i2s1_mclkout_tx_sel::write(CruClksel::i2s_mclkout_sel::mclk_i2s_8ch);
	// wr 0xfdd2013c 0x80000000
	// clears bit 15 -> i2s1_mclkout_tx_sel = mclk_i2s1_8ch_tx

	// CruClksel::i2s1_mclkout_rx_sel::write(CruClksel::i2s_mclkout_sel::xin_osc0_half);
	// wr 0xfdd20144 0x80008000
	// sets bit 15 -> i2s1_mclkout_rx_sel is xin

	// Set the clock divider for cpll
	// We will probably need to use the fractional divider to get this more exact
	// GPLL is 1200MHz,
	// maybe gpll_100m is 100MHz?
	// /8 -> 12MHz
	CruClksel::i2s1_8ch_tx_src_div::write(0x8);
	// wr 0xfdd2013c 0x007f0064

	// CruClksel::i2s1_8ch_rx_src_div::write(0x8);
	// wr 0xfdd20144 0x007f0064

	// Select the I2S clock source to be gpll
	// printf("Set I2S clock source\n\r");
	CruClksel::i2s1_8ch_tx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);
	// wr 0xfdd2013c 0x03000000

	// CruClksel::i2s1_8ch_rx_src_sel::write(CruClksel::clk_i2s_8ch_src_sel::clk_gpll_mux);
	// wr 0xfdd20144 0x03000000

	// Select the MCLK source clock to the integral divided clock
	CruClksel::mclk_i2s1_8ch_tx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	// wr 0xfdd2013c 0x0c000000

	// CruClksel::mclk_i2s1_8ch_rx_sel::write(CruClksel::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	// wr 0xfdd20144 0x0c000000

	CruGate::mclk_i2s1_8ch_tx_en::write(CruGate::cru_clock_enable);
	// wr 0xfdd20318 0x04000000

	CruGate::i2s1_mclkout_tx_en::write(CruGate::cru_clock_enable);
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
	// HW::I2S1->enable_DMA();
	HW::I2S1->tdm_tx8_mode();
	// HW::I2S1->tdm_rx6_mode();
	HW::I2S1->master_tx();

	printf("Select TX to MCLK pin\n");
	SysGrf::i2s1_mclk_sel::write(SysGrf::con1_i2s1_mclk_sel::i2s1_mclk_tx);
	SysGrf::i2s1_mclk_tx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_cru);
	// SysGrf::i2s1_mclk_rx_oe::write(SysGrf::con2_i2s1_mclk_oe::from_ext_chip);

	// Setup DMA

	// Setup interrupt
	mdrivlib::InterruptManager::register_and_start_isr(IRQ::I2S1_8CH_IRQ, 0, 0, [] {
		printf("I2S1 IRQ\n");
		HW::I2S1->clear_tx_underrun();
	});

	// Enable IRQs
	printf("Enable IRQ\n");
	mdrivlib::IRQ_init();
	enable_irq();

	printf("Setting pin mux\n");
	GrfIofunc::i2s1_iomux_sel_m1::write(GrfIofunc::choice_iomux3::m1);

	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_7::I2S1_SCLKTXM1);
	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_6::I2S1_MCLKM1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_1::I2S1_SDO0M1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_2::I2S1_SDI0M1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_0::I2S1_LRCKTXM1);
	HW::SYS->gpio4_a_h.write(Rockchip::GPIO4A_IOMUX_H_SEL_7::I2S1_LRCKRXM1);

	printf("Enable TX XFER\n");
	HW::I2S1->XFER = 0b01;

	HW::I2S1->enable_ISR();

	// HW::I2S1->TXDR = 0x00111100;

	Console::init();

	// uint32_t i = 0x55AA00;
	while (true) {
		Console::process();

		// HW::I2S1->TXDR = i++;
		// i = HW::I2S1->RXDR;

		asm("nop");
	}
}
