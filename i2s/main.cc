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

	// mdrivlib::InterruptManager::register_and_start_isr(GPIO4IRQ, 0, 0, [] {
	// });

	// HW::PMU->gpio0_b_h.write(Rockchip::GPIO0B_IOMUX_H_SEL_7::PWM0_M0);

	// Enable IRQs
	enable_irq();
	printf("\nEnable IRQ\n");

	Console::init();

	while (true) {
		Console::process();

		asm("nop");
	}
}
