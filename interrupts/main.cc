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

constexpr uint32_t GPIO4IRQ = 69;
constexpr uint32_t PWM0IRQ = 114;

void dump_sys_state();

CONSOLE_COMMAND_DEF(pin, "pin", CONSOLE_INT_ARG_DEF(onoff, "1=on 0=off"));
static void pin_command_handler(const pin_args_t *args) {
	if (args->onoff)
		HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
	else
		HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);
}

CONSOLE_COMMAND_DEF(oneshot, "oneshot", CONSOLE_INT_ARG_DEF(num, "Which PWM to fire a one-shot"));
static void oneshot_command_handler(const oneshot_args_t *args) {
	if (args->num == 0)
		HW::PWM0->chan[0].control = 0x01;
}

CONSOLE_COMMAND_DEF(st, "status", CONSOLE_INT_ARG_DEF(dummy, "Not used"));
static void st_command_handler(const st_args_t *args) {
	printf("GPIO4: en: %u IRQstat:%u intr_status:%x (raw: %x)\n",
		   GIC_GetEnableIRQ(GPIO4IRQ),
		   GIC_GetIRQStatus(GPIO4IRQ),
		   HW::GPIO4->intr_status,
		   HW::GPIO4->intr_rawstatus);

	printf("PWM0: en: %u IRQstat:%u int_en:%x intr_status:%x\n",
		   GIC_GetEnableIRQ(PWM0IRQ),
		   GIC_GetIRQStatus(PWM0IRQ),
		   HW::PWM0->int_en,
		   HW::PWM0->intsts);
}

int main() {
	console_command_register(oneshot);
	console_command_register(st);
	console_command_register(pin);

	dump_sys_state();

	printf("\n******************************\n");
	printf("IRQ Example\n");
	printf("Connect GPIO0.C5 to GPIO4.C0 to see nested interrupts working.\n");
	printf("Fire one-shot with command `oneshot 0`\n");
	printf("Set GPIO0.C5 high/low with `pin 0` or `pin 1`. There is an interrupt when the pin goes high\n");

	using namespace RockchipPeriph;

	mdrivlib::IRQ_init();

	////////////////////////////////////////
	// GPIO4 C0 interrupt

	double m = 1;
	mdrivlib::InterruptManager::register_and_start_isr(GPIO4IRQ, 0, 0, [&m] {
		printf("GPIO 4 IRQ\n");
		for (int i = 0; i < 5; i++) {
			m = m * 2.;
			printf("GPIO4: %g\n", m);
		}
		HW::GPIO4->intr_eoi_H = Gpio::masked_set_bit(Gpio::C(0));
	});

	// Set up GPIO4_C0 as input
	HW::GPIO4->dir_H = Gpio::masked_clr_bit(Gpio::C(0));

	// disable interrupt
	HW::GPIO4->intr_en_H = Gpio::masked_clr_bit(Gpio::C(0));

	// polarity High
	HW::GPIO4->intr_pol_H = Gpio::masked_set_bit(Gpio::C(0));

	// Clear pending
	HW::GPIO4->intr_eoi_H = Gpio::masked_set_bit(Gpio::C(0));

	// Int type = edge
	HW::GPIO4->intr_type_H = Gpio::masked_set_bit(Gpio::C(0));

	// disable debounce
	HW::GPIO4->debounce_H = Gpio::masked_clr_bit(Gpio::C(0));

	// // unmask interrupt
	HW::GPIO4->intr_mask_H = Gpio::masked_clr_bit(Gpio::C(0));

	// enable interrupt
	HW::GPIO4->intr_en_H = Gpio::masked_set_bit(Gpio::C(0));

	////////////////////////////////////////
	// PWM interrupt

	auto pwm_irqn = 114;

	mdrivlib::InterruptManager::register_and_start_isr(pwm_irqn, 3, 3, [] {
		printf("PWM IRQ\n");
		double x = 2.;
		while ((long)x < 0x10000000) {
			printf("PWM: %g\n", x);
			// x = x * 1.0005;
			x = x * 2;

			if (x > 1023 && x < 1025) {
				printf("Set GPIO0 C5 high\n");
				HW::GPIO0->high(Gpio::Port::C, 5);
			}
		}
		// clear:
		HW::PWM0->intsts = 0x1;
	});

	HW::PMU->gpio0_b_h.write(Rockchip::GPIO0B_IOMUX_H_SEL_7::PWM0_M0);
	HW::PWM0->chan[0].period = 0xFFFF;
	HW::PWM0->chan[0].duty = 2048;
	HW::PWM0->int_en = 0x1;

	///////////////////////////////

	// Enable IRQs
	enable_irq();
	printf("\nEnable IRQ\n");

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));
	HW::GPIO0->low(Gpio::Port::C, 5);

	Console::init();

	printf("\nFiring one shot\n");

	// Fire the oneshot
	HW::PWM0->chan[0].control = 0x01;

	while (true) {
		Console::process();

		asm("nop");
	}
}

bool BIT(auto reg, unsigned pos) {
	return (reg & (1 << pos)) ? true : false;
}

void dump_sys_state() {
	// Dump some useful info:
	auto el = get_current_el();
	printf("\nCurrent EL: %d\n", el);

	auto hcr = read_hcr_el2();
	printf("HCR = %lx: IMO = %u, FMO = %u\n", hcr, BIT(hcr, 4), BIT(hcr, 3));

	printf("VBAR_EL2 = %08lx\n", read_vbar_el2());
	printf("DAIF = %08x\n", read_daif());

	auto spsr = read_spsr_el2();
	printf("spsr = 0x%lx, IRQ masked = %u, FIQ masked = %u\n", spsr, BIT(spsr, 7), BIT(spsr, 6));
}
