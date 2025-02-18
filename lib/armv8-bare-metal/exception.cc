/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception handler                                                 */
/*                                                                    */
/**********************************************************************/

#include "exception.h"
#include "../../drivers/gic.hh"
#include "../../drivers/gpio.hh"
#include "../../drivers/serial.hh"
#include <stdio.h>

void dump_frame(exception_frame *exc) {
	printf("An exception occur:\n");
	printf("exc_type: %016lx", (unsigned long)exc->exc_type);
	printf("\nESR: %016lx", (unsigned long)exc->exc_esr);
	printf("  SP: %016lx", (unsigned long)exc->exc_sp);
	printf(" ELR: %016lx", (unsigned long)exc->exc_elr);
	printf(" SPSR: %016lx", (unsigned long)exc->exc_spsr);
	printf("\n x0: %016lx", (unsigned long)exc->x0);
	printf("  x1: %016lx", (unsigned long)exc->x1);
	printf("  x2: %016lx", (unsigned long)exc->x2);
	printf("  x3: %016lx", (unsigned long)exc->x3);
	printf("\n x4: %016lx", (unsigned long)exc->x4);
	printf("  x5: %016lx", (unsigned long)exc->x5);
	printf("  x6: %016lx", (unsigned long)exc->x6);
	printf("  x7: %016lx", (unsigned long)exc->x7);
	printf("\n x8: %016lx", (unsigned long)exc->x8);
	printf("  x9: %016lx", (unsigned long)exc->x9);
	printf(" x10: %016lx", (unsigned long)exc->x10);
	printf(" x11: %016lx", (unsigned long)exc->x11);
	printf("\nx12: %016lx", (unsigned long)exc->x12);
	printf(" x13: %016lx", (unsigned long)exc->x13);
	printf(" x14: %016lx", (unsigned long)exc->x14);
	printf(" x15: %016lx", (unsigned long)exc->x15);
	printf("\nx16: %016lx", (unsigned long)exc->x16);
	printf(" x17: %016lx", (unsigned long)exc->x17);
	printf(" x18: %016lx", (unsigned long)exc->x18);
	printf(" x19: %016lx", (unsigned long)exc->x19);
	printf("\nx20: %016lx", (unsigned long)exc->x20);
	printf(" x21: %016lx", (unsigned long)exc->x21);
	printf(" x22: %016lx", (unsigned long)exc->x22);
	printf(" x23: %016lx", (unsigned long)exc->x23);
	printf("\nx24: %016lx", (unsigned long)exc->x24);
	printf(" x25: %016lx", (unsigned long)exc->x25);
	printf(" x26: %016lx", (unsigned long)exc->x26);
	printf(" x27: %016lx", (unsigned long)exc->x27);
	printf("\nx28: %016lx", (unsigned long)exc->x28);
	printf(" x29: %016lx", (unsigned long)exc->x29);
	printf(" x30: %016lx", (unsigned long)exc->x30);
}

void common_trap_handler(exception_frame *exc) {
	printf("\nException Handler: Type = ");

	if ((exc->exc_type & 0xff) == AARCH64_EXC_SYNC_SPX) {
		printf("SError in Aarch64, SPx\n");

		dump_frame(exc);

		printf("Press a key to jump to main())\n");
		while (!rk_getc())
			;

		// TODO: reset SPSR and other system state?
		asm("b main");

		return;
	}

	else if ((exc->exc_type & 0xff) == AARCH64_EXC_IRQ_SPX)
	{
		printf("IRQ in AARCH64, SPx\n");
		auto irq = GIC_AcknowledgePendingGroup1();

		printf("Ack IRQ# %lu, status => %x\n", (unsigned long)irq, GIC_GetIRQStatus(69));

		if (irq == 69) {
			using namespace RockchipPeriph;

			if (HW::GPIO4->intr_status & (1 << 16)) {
				printf("Clear GPIO4 pin C0 interrupt. Gpio4 intr status %x (%x) => ",
					   HW::GPIO4->intr_rawstatus,
					   HW::GPIO4->intr_status);
				// HW::GPIO4->intr_en_H = Gpio::masked_clr_bit(Gpio::C(0));
				HW::GPIO4->intr_eoi_H = Gpio::masked_set_bit(Gpio::C(0));
				// HW::GPIO4->intr_en_H = Gpio::masked_set_bit(Gpio::C(0));
				printf("%x (%x). IRQstatus: %u\n",
					   HW::GPIO4->intr_rawstatus,
					   HW::GPIO4->intr_status,
					   GIC_GetIRQStatus(69));
			}
		}

		// GIC_DeactivateInterrupt(irq);
		GIC_EndInterruptGroup1(irq);
		return;
	}

	else
	{
		printf("Unknown -- not handling\n");
		return;
	}
}
