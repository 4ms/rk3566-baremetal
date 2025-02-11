#include "armv8-bare-metal/aarch64.h"
#include "armv8-bare-metal/gic_v3.h"
#include "gpio.hh"
// #include "mmu.h"
#include "console.hh"
#include <cstdio>

constexpr uint64_t SystemCntFreq = 25'000'000;
// constexpr uint32_t GPIO0IRQ = 65;
constexpr uint32_t GPIO4IRQ = 69;

int main() {
	using namespace RockchipPeriph;

	printf("\n");
	auto el = get_current_el();
	printf("Current EL: %d\n", el);

	// printf("Enabling MMU:\n");
	// enable_mmu();

	printf("Enabling IRQ:\n");
	gic_v3_initialize();

	Console::init();

	printf("Config GPIO0 IRQ:\n");
	gicd_config(GPIO4IRQ, GIC_GICD_ICFGR_EDGE);
	printf("gicd_set_priority:\n");
	gicd_set_priority(GPIO4IRQ, 1 << GIC_PRI_SHIFT); /* Set priority */
	printf("gicd_set_target:\n");
	gicd_set_target(GPIO4IRQ, 0x1); /* processor 0 */
	printf("gicd_clear_pending:\n");
	gicd_clear_pending(GPIO4IRQ);
	printf("gicd_enable_int:\n");
	gicd_enable_int(GPIO4IRQ);

	// printf("RVBAR_EL1 = %lx\n", raw_read_rvbar_el1());
	printf("VBAR_EL2 = %08lx\n", raw_read_vbar_el2());
	printf("DAIF = %08x\n", raw_read_daif());

	// // Disable the timer
	// disable_cntv();
	// printf("Disable the timer, CNTV_CTL_EL0 = %x\n", raw_read_cntv_ctl());
	// printf("System Frequency: CNTFRQ_EL0 = %x\n", raw_read_cntfrq_el0());

	// // Pause 2 sec.
	// auto ticks = SystemCntFreq * 2;
	// // Get value of the current timer
	// auto current_cnt = raw_read_cntvct_el0();
	// printf("Current counter: CNTVCT_EL0 = %lx\n", current_cnt);

	// // Set the interrupt in Current Time + TimerTick
	// raw_write_cntv_cval_el0(current_cnt + ticks);
	// printf("Assert Timer IRQ after 2 sec: CNTV_CVAL_EL0 = %lx\n", raw_read_cntv_cval_el0());

	// // Enable the timer
	// enable_cntv();
	// printf("Enable the timer, CNTV_CTL_EL0 = %x\n", raw_read_cntv_ctl());

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));
	HW::GPIO0->high(Gpio::Port::C, 5);
	HW::GPIO0->low(Gpio::Port::C, 5);

	// Set up GPIO4_C0 as input
	HW::GPIO4->dir_H = Gpio::masked_clr_bit(Gpio::C(0));

	// enable interrupt
	HW::GPIO4->intr_en_H = Gpio::masked_set_bit(Gpio::C(0));

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

	// Enable IRQ
	enable_irq();
	printf("\nEnable IRQ, DAIF = %x\n", raw_read_daif());

	printf("Before: %08x %08x\n", HW::GPIO4->intr_status, HW::GPIO4->intr_rawstatus);

	HW::GPIO0->high(Gpio::Port::C, 5);
	volatile int dly = 2'000'000;
	while (dly--)
		;
	printf("After high: %08x %08x\n",
		   HW::GPIO4->intr_status,
		   HW::GPIO4->intr_rawstatus); // pin C0 =  bit 0x0001'0000 = bit 16

	HW::GPIO0->low(Gpio::Port::C, 5);
	dly = 2'000'000;
	while (dly--)
		;
	printf("After low: %08x %08x\n",
		   HW::GPIO4->intr_status,
		   HW::GPIO4->intr_rawstatus); // pin C0 =  bit 0x0001'0000 = bit 16

	while (true) {
		Console::process();

		// wfi();
		// printf("INT\n");

		if (HW::GPIO4->intr_status & (1 << 16)) {
			printf("Clear\n");
			HW::GPIO4->intr_eoi_H = Gpio::masked_set_bit(Gpio::C(0));
		}
	}
}
