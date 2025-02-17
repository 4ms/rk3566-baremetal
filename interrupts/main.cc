#include "aarch64_system_reg.hh"
#include "console.hh"
#include "gic.hh"
#include "gpio.hh"
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

	auto hcr = read_hcr_el2();
	printf("HCR = %lx: IMO = %u, FMO = %u\n", hcr, BIT(hcr, 4), BIT(hcr, 3));

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
	GIC_SetEOIModeTwoStep(true);

	auto spsr = read_spsr_el2();
	printf("spsr = 0x%lx, IRQ masked = %u, FIQ masked = %u\n", spsr, BIT(spsr, 7), BIT(spsr, 6));

	// Enable Group1NS:
	printf("Enable Group1NS in GICD CTRL\n");
	GIC_EnableGroup1NS();

	printf("Config GPIO4 as level/edge:\n");
	GIC_SetConfiguration(GPIO4IRQ, InterruptConfig::Edge);

	printf("Disable IRQ %u\n", GPIO4IRQ);
	GIC_DisableIRQ(GPIO4IRQ);
	printf("readback GPIO4 IRQ enabled bit: %u\n", GIC_GetEnableIRQ(GPIO4IRQ));

	auto pri = 1 << 0;
	printf("Set priority for IRQ %u to %u\n", GPIO4IRQ, pri);
	GIC_SetPriority(GPIO4IRQ, pri);

	printf("Clear pending IRQ %u:\n", GPIO4IRQ);
	GIC_ClearPendingIRQ(GPIO4IRQ);

	printf("Enable IRQ %u:\n", GPIO4IRQ);
	GIC_EnableIRQ(GPIO4IRQ);

	printf("Set Routing mode to 1\n");
	GIC_SetRoutingMode(1);

	printf("VBAR_EL2 = %08lx\n", read_vbar_el2());
	printf("DAIF = %08x\n", read_daif());

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
	printf("\nEnable IRQ, DAIF = %x\n", read_daif());

	HW::GPIO0->high(Gpio::Port::C, 5);
	printf("Set GPIO0 C5 high\n");

	Console::init();

	printf("\n");

	unsigned pp = 0;
	while (true) {
		Console::process();

		asm("nop");

		// if (pp++ >= 4'000'000) {
		// 	GIC_SetInterfacePriorityMask(pmr);
		// 	printf("en: %u stat:%u pri:%u\n",
		// 		   GIC_GetEnableIRQ(GPIO4IRQ),
		// 		   GIC_GetIRQStatus(GPIO4IRQ),
		// 		   GIC_GetPriority(GPIO4IRQ));
		// 	auto el = get_current_el();
		// 	printf("Current EL: %d\n", el);
		// 	pp = 0;
		// }

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
