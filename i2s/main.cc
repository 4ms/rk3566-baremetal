#include "drivers/aarch64_system_reg.hh"
#include "drivers/console.hh"
#include "drivers/gic.hh"
#include "drivers/gpio.hh"
#include "drivers/grf.hh"
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
	console_command_register(pin);

	using namespace RockchipPeriph;

	mdrivlib::IRQ_init();

	// Pins:
	// (22) GPIO3_C6: I2S1_MCLK_M1
	// (12) GPIO3_C7: I2S1_SCLK_TX_M1
	// (35) GPIO3_D0: I2S1_LRCK_TX_M1
	// (40) GPIO3_D1: I2S1_SDO0_M1
	// (38) GPIO3_D2: I2S1_SDI0_M1
	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_6::I2S1_MCLKM1);
	HW::SYS->gpio3_c_h.write(Rockchip::GPIO3C_IOMUX_H_SEL_7::I2S1_SCLKTXM1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_0::I2S1_LRCKTXM1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_1::I2S1_SDO0M1);
	HW::SYS->gpio3_d_l.write(Rockchip::GPIO3D_IOMUX_L_SEL_2::I2S1_SDI0M1);

	// mdrivlib::InterruptManager::register_and_start_isr(GPIO4IRQ, 0, 0, [] {
	// });

	// Enable IRQs
	enable_irq();
	printf("\nEnable IRQ\n");

	Console::init();

	while (true) {
		Console::process();

		asm("nop");
	}
}
