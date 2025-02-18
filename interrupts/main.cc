#include "armv8-bare-metal/aarch64.h"
#include "armv8-bare-metal/gic_v3.h"

#include "aarch64_system_reg.hh"
#include "gpio.hh"
// #include "mmu.h"
#include "console.hh"
#include "gic.hh"
#include "serial.hh"
#include <cstdio>

constexpr uint32_t GPIO4IRQ = 69;

bool BIT(auto reg, unsigned pos) {
	return (reg & (1 << pos)) ? true : false;
}

int main() {
	using namespace RockchipPeriph;

	// asm(".equ UART_THR, 0xfe660000\nldr x0, =UART_THR\n mov x1, #70\n str x1, [x0]\n");

	printf("\n");
	auto el = get_current_el();
	printf("Current EL: %d\n", el);

	uint64_t scr = 0x238; // raw_read_scr_el3();
	printf("Assuming SCR is 0x238 (check printf from TFA)\n");
	printf("SCR = %lx: IRQ = %u, FIQ = %u\n", scr, BIT(scr, 1), BIT(scr, 2));

	auto hcr = raw_read_hcr_el2();
	printf("HCR = %lx: IMO = %u, FMO = %u\n", hcr, BIT(hcr, 4), BIT(hcr, 3));

	printf("Enabling IRQ:\n");

	// Wake up
	printf("Waking CPU GICR:\n");
	HW::GICRedist->Wake();

	uint64_t pmr = 0xFF;
	printf("Set PMR (min priority) via GICC to %lu\n", pmr);
	GIC_SetInterfacePriorityMask(pmr);
	printf("Read PMR back %u\n", GIC_GetInterfacePriorityMask());

	printf("Set BPR (priority point)\n");
	GIC_SetBinaryPoint(3); // Group priority: [7:3], Subpriority [2:0]
	GIC_SetBinaryPointGroup1(2);
	printf("Read back BPR: bpr0:%x bpr1:%x\n", GIC_GetBinaryPoint(), GIC_GetBinaryPointGroup1());

	printf("Set EOI mode to single step\n");
	GIC_SetEOIModeTwoStep(false);

	auto spsr = read_spsr_el2();
	printf("spsr = 0x%lx, IRQ masked = %u, FIQ masked = %u\n", spsr, BIT(spsr, 7), BIT(spsr, 6));

	// Enable Group1NS:
	printf("Enable Group1NS in GICD CTRL\n");
	GIC_EnableGroup1NS();

	auto type = GIC_GICD_ICFGR_EDGE;
	printf("Config GPIO4 as %s\n", type == GIC_GICD_ICFGR_LEVEL ? "level" : "edge");
	GIC_SetConfiguration(GPIO4IRQ, type);

	printf("Disable IRQ %u\n", GPIO4IRQ);
	GIC_DisableIRQ(GPIO4IRQ);

	auto pri = 1 << 0;
	printf("Set priority for IRQ %u to %u\n", GPIO4IRQ, pri);
	GIC_SetPriority(GPIO4IRQ, pri);

	printf("Clear pending IRQ %u:\n", GPIO4IRQ);
	GIC_ClearPendingIRQ(GPIO4IRQ);

	printf("Enable IRQ %u:\n", GPIO4IRQ);
	GIC_EnableIRQ(GPIO4IRQ);

	printf("Set Routing mode to 1\n");
	GIC_SetRoutingMode(1);

	printf("VBAR_EL2 = %08lx\n", raw_read_vbar_el2());
	printf("DAIF = %08x\n", raw_read_daif());

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));
	HW::GPIO0->high(Gpio::Port::C, 5);
	HW::GPIO0->low(Gpio::Port::C, 5);

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

	// Enable IRQ
	enable_irq();
	printf("\nEnable IRQ, DAIF = %x\n", raw_read_daif());

	HW::GPIO0->high(Gpio::Port::C, 5);
	volatile int dly = 2'000'000;
	while (dly--)
		;

	Console::init();

	printf("\n");

	unsigned pp = 0;
	while (true) {
		Console::process();

		asm("nop");

		if (pp++ >= 4'000'000) {
			// GIC_SetInterfacePriorityMask(pmr);
			printf("EL: %d, en: %u IRQstat:%u intr_status:%x (%x)\n",
				   get_current_el(),
				   GIC_GetEnableIRQ(GPIO4IRQ),
				   GIC_GetIRQStatus(GPIO4IRQ),
				   HW::GPIO4->intr_rawstatus,
				   HW::GPIO4->intr_status);
			pp = 0;
		}

		// volatile int dly = 1'000'000;
		// while (dly--)
		// 	;

		// if (*gic_pending) {
		// 	printf("pending gpio4 interrupt %08x\n", *gic_pending);
		// 	auto irq = GIC_AcknowledgePending();
		// 	if (irq != GPIO4IRQ) {
		// 		printf("ack %d, not %d\n", irq, GPIO4IRQ);
		// 		GIC_EndInterrupt(irq);
		// 	} else
		// 		GIC_EndInterrupt(GPIO4IRQ);
		// }

		// if (HW::GPIO4->intr_status & (1 << 16)) {
		// 	printf("Clear\n");
		// 	HW::GPIO4->intr_eoi_H = Gpio::masked_set_bit(Gpio::C(0));
		// }
	}
}
