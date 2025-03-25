#include "drivers/console.hh"
#include "drivers/cru.hh"
#include "drivers/gic.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
#include "drivers/i2s.hh"
#include "drivers/interrupt.hh"
#include "drivers/irq_init.hh"
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

	// Pins:
	// (22) GPIO3_C6: I2S1_MCLK_M1
	// (12) GPIO3_C7: I2S1_SCLK_TX_M1
	// (35) GPIO3_D0: I2S1_LRCK_TX_M1
	// (40) GPIO3_D1: I2S1_SDO0_M1
	// (38) GPIO3_D2: I2S1_SDI0_M1

	// Setup I2S
	HW::I2S1->XFER = 0;
	HW::I2S1->CLR = 1;
	while (HW::I2S1->CLR != 0)
		;
	HW::I2S1->tdm_tx8_mode();
	HW::I2S1->master_tx();

	printf("Select TX to MCLK pin\n");
	GRF_SOC::i2s1_mclk_sel::write(GRF_SOC::con1_i2s1_mclk_sel::i2s1_mclk_tx);
	GRF_SOC::i2s1_mclk_tx_oe::write(GRF_SOC::con2_i2s1_mclk_oe::from_cru);
	GRF_SOC::i2s1_mclk_rx_oe::write(GRF_SOC::con2_i2s1_mclk_oe::from_ext_chip);

	// Clocks:

	// Set the clock divider for cpll
	// TODO: need to calculate this so we hit 48kHz * 256 = 12.288MHz
	// Estimate cPLL is at 50MHz to start??? /4 is close
	// We will probably need to use the fractional divider to get this more exact
	// printf("Set clock divider (reg %08x)\n", Cru::CLKSEL::reg(Cru::CLKSEL::I2S1_tx));
	Cru::i2s1_8ch_tx_src_div::write(0x13);
	Cru::i2s1_8ch_rx_src_div::write(0x13);

	// Select the I2S clock source to be gpll
	// printf("Set I2S clock source\n\r");
	// wr 0x0FDD2013C bits 9:8 to 01
	Cru::i2s1_8ch_tx_src_sel::write(Cru::clk_i2s_8ch_src_sel::clk_cpll_mux);
	Cru::i2s1_8ch_rx_src_sel::write(Cru::clk_i2s_8ch_src_sel::clk_cpll_mux);

	// Select the MCLK source clock to the integral divided clock
	// printf("Set I2S MCLK clock source\n\r");
	// wr 0x0FDD2013C bits 11:10 to 00
	Cru::mclk_i2s1_8ch_tx_sel::write(Cru::mclk_i2s_8ch_sel::clk_i2s_8ch_src);
	Cru::mclk_i2s1_8ch_rx_sel::write(Cru::mclk_i2s_8ch_sel::clk_i2s_8ch_src);

	// Connect MCLKOUT to I2S1 TX mclk
	printf("Connect MCLKOUT\n\r");
	// wr 0x0FDD2013C 0x80000000
	Cru::i2s1_mclkout_tx_sel::write(Cru::i2s_mclkout_sel::mclk_i2s_8ch);
	Cru::i2s1_mclkout_rx_sel::write(Cru::i2s_mclkout_sel::xin_osc0_half);

	// Setup DMA
	// Setup interrupt
	// mdrivlib::InterruptManager::register_and_start_isr(GPIO4IRQ, 0, 0, [] {
	// });

	// Enable IRQs
	// mdrivlib::IRQ_init();
	// enable_irq();
	// printf("\nEnable IRQ\n");

	printf("Enabling pins\n\r");
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
