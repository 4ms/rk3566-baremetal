#include "armv8-bare-metal/aarch64.h"
#include "armv8-bare-metal/gic_v3.h"
#include "gpio.hh"
// #include "mmu.h"
#include "console.hh"
#include "gic.hh"
#include <cstdio>

constexpr uint64_t SystemCntFreq = 25'000'000;
// constexpr uint32_t GPIO0IRQ = 65;
constexpr uint32_t GPIO4IRQ = 69;

bool BIT(auto reg, unsigned pos) {
	return (reg & (1 << pos)) ? true : false;
}

int main() {
	using namespace RockchipPeriph;

	printf("\n");
	auto el = get_current_el();
	printf("Current EL: %d\n", el);

	uint64_t scr = 0x238; // raw_read_scr_el3();
	printf("Assuming SCR is 0x238 (check printf from TFA)\n");
	printf("SCR = %lx: IRQ = %u, FIQ = %u\n", scr, BIT(scr, 1), BIT(scr, 2));

	auto hcr = raw_read_hcr_el2();
	printf("HCR = %lx: IMO = %u, FMO = %u\n", hcr, BIT(hcr, 4), BIT(hcr, 3));

	printf("Enabling IRQ:\n");
	// gic_v3_initialize();

	// Wake
	HW::GICRedist->Wake();

	uint64_t pmr = 0xFF;
	printf("Set PMR (min priority) via GICC to %lu\n", pmr);
	GIC_SetInterfacePriorityMask(pmr);
	printf("Read PMR back %lu\n", read_icc_pmr_el1());

	/////////////////

	printf("Set BPR (priority point)\n");
	write_icc_bpr0_el1(2); // Group priority: [7:3], Subpriority [2:0]
	write_icc_bpr1_el1(3);
	printf("Read back BPR: 0:%lx 1:%lx\n", read_icc_bpr0_el1(), read_icc_bpr1_el1());

	auto spsr = raw_read_spsr_el2();
	printf("spsr = 0x%lx, IRQ masked = %u, FIQ masked = %u\n", spsr, BIT(spsr, 7), BIT(spsr, 6));

	// Enable Group1NS:
	printf("Enable Group1NS in GICD CTRL\n");
	GIC_EnableGroup1NS();

	printf("Not Config GPIO4 IRQ:\n");
	// gicd_config(GPIO4IRQ, GIC_GICD_ICFGR_LEVEL);

	printf("Disable IRQ %u\n", GPIO4IRQ);
	GIC_DisableIRQ(GPIO4IRQ);
	printf("readback GPIO4 IRQ enabled bit: %u\n", GIC_GetEnableIRQ(GPIO4IRQ));

	auto pri = 1 << 0;
	printf("gicd_set_priority: %u\n", pri);
	GIC_SetPriority(GPIO4IRQ, pri);

	// auto target = 1;
	// printf("gicd_set_target: %u\n", target);
	// GIC_SetTarget(GPIO4IRQ, target);

	// printf("gicd_clear_pending:\n");
	// GIC_ClearPendingIRQ(GPIO4IRQ);

	printf("gicd_enable_int:\n");
	// gicd_enable_int(GPIO4IRQ);
	GIC_EnableIRQ(GPIO4IRQ);
	printf("readback GPIO4 IRQ enabled bit: %u\n", GIC_GetEnableIRQ(GPIO4IRQ));

	printf("Setup SGI: enable and set group\n");
	HW::GICRedist->ISENABLER[0] = (1 << 2);
	HW::GICRedist->IGROUPR[0] = (1 << 2);
	auto sgi_group = HW::GICRedist->IGROUPR[0];
	printf("SGI group %x\n", sgi_group);
	auto sgi_enabled = HW::GICRedist->ISENABLER[0];
	printf("SGI enabled %x\n", sgi_enabled);

	GIC_SendSGI(2, 1, 0);

	auto enabled = reinterpret_cast<uint32_t *>(0xfd400108);
	auto gic_pending = reinterpret_cast<uint32_t *>(0xfd400208);

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
	enable_fiq();
	printf("\nEnable IRQ, DAIF = %x\n", raw_read_daif());

	HW::GPIO0->high(Gpio::Port::C, 5);
	volatile int dly = 2'000'000;
	while (dly--)
		;

	printf("enabled %d\n", *enabled);
	printf("pending %d\n", *gic_pending);

	Console::init();

	while (true) {
		Console::process();

		// volatile int dly = 8'000'000;
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
