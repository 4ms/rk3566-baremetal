/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Exception handler                                                 */
/*                                                                    */
/**********************************************************************/

#include "exception.h"
#include "gic_v3.h"
// #include "psw.h"
// #include "uart.h"
#include <stdio.h>

extern void timer_handler(void);

void handle_exception(exception_frame *exc) {
	printf("An exception occur:\n");
	printf("exc_type: %016llx", exc->exc_type);
	printf("\nESR: %016llx", exc->exc_esr);
	printf("  SP: %016llx", exc->exc_sp);
	printf(" ELR: %016llx", exc->exc_elr);
	printf(" SPSR: %016llx", exc->exc_spsr);
	printf("\n x0: %016llx", exc->x0);
	printf("  x1: %016llx", exc->x1);
	printf("  x2: %016llx", exc->x2);
	printf("  x3: %016llx", exc->x3);
	printf("\n x4: %016llx", exc->x4);
	printf("  x5: %016llx", exc->x5);
	printf("  x6: %016llx", exc->x6);
	printf("  x7: %016llx", exc->x7);
	printf("\n x8: %016llx", exc->x8);
	printf("  x9: %016llx", exc->x9);
	printf(" x10: %016llx", exc->x10);
	printf(" x11: %016llx", exc->x11);
	printf("\nx12: %016llx", exc->x12);
	printf(" x13: %016llx", exc->x13);
	printf(" x14: %016llx", exc->x14);
	printf(" x15: %016llx", exc->x15);
	printf("\nx16: %016llx", exc->x16);
	printf(" x17: %016llx", exc->x17);
	printf(" x18: %016llx", exc->x18);
	printf(" x19: %016llx", exc->x19);
	printf("\nx20: %016llx", exc->x20);
	printf(" x21: %016llx", exc->x21);
	printf(" x22: %016llx", exc->x22);
	printf(" x23: %016llx", exc->x23);
	printf("\nx24: %016llx", exc->x24);
	printf(" x25: %016llx", exc->x25);
	printf(" x26: %016llx", exc->x26);
	printf(" x27: %016llx", exc->x27);
	printf("\nx28: %016llx", exc->x28);
	printf(" x29: %016llx", exc->x29);
	printf(" x30: %016llx", exc->x30);
}

void irq_handle(exception_frame *exc) {
	// psw_t psw;
	// irq_no irq;
	// int rc;

	// psw_disable_and_save_interrupt(&psw);
	// rc = gic_v3_find_pending_irq(exc, &irq);
	// if (rc != IRQ_FOUND) {
	// 	printf("IRQ not found!\n");
	// 	psw_restore_interrupt(&psw);
	// } else {
	// 	printf("IRQ found: %llx\n", irq);
	// }
	// gicd_disable_int(irq); /* Mask this irq */
	// gic_v3_eoi(irq);	   /* Send EOI for this irq line */
	// timer_handler();
	// gicd_enable_int(irq); /* unmask this irq line */

	// psw_restore_interrupt(&psw);
}

char rk_getc();

void common_trap_handler(exception_frame *exc) {
	printf("\nException Handler! (");
	// handle_exception(exc);

	if ((exc->exc_type & 0xff) == AARCH64_EXC_SYNC_SPX) {
		printf("AARCH64_EXC_SYNC_SPX)\n");
		handle_exception(exc);
		while (!rk_getc())
			;
		asm("b main");
		/*
				ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, 1);
				enable_irq();
				hal_handle_exception(exc);
				disable_irq();
				ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, -1);
		*/
	}

	if ((exc->exc_type & 0xff) == AARCH64_EXC_IRQ_SPX) {
		printf("AARCH64_EXC_IRQ_SPX)\n");
		irq_handle(exc);
	}
	return;
}
