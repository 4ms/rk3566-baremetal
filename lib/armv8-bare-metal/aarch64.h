/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 definitions                                               */
/*                                                                    */
/**********************************************************************/
#if !defined(_AARCH64_H)
#define _AARCH64_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CurrentEL, Current Exception Level */
#define CURRENT_EL_MASK 0x3
#define CURRENT_EL_SHIFT 2

/* DAIF, Interrupt Mask Bits */
#define DAIF_DBG_BIT (1 << 3) /* Debug mask bit */
#define DAIF_ABT_BIT (1 << 2) /* Asynchronous abort mask bit */
#define DAIF_IRQ_BIT (1 << 1) /* IRQ mask bit */
#define DAIF_FIQ_BIT (1 << 0) /* FIQ mask bit */

/*
 * Interrupt flags
 */
#define AARCH64_DAIF_FIQ (1) /* FIQ */
#define AARCH64_DAIF_IRQ (2) /* IRQ */

/* Timer */
#define CNTV_CTL_ENABLE (1 << 0)  /* Enables the timer */
#define CNTV_CTL_IMASK (1 << 1)	  /* Timer interrupt mask bit */
#define CNTV_CTL_ISTATUS (1 << 2) /* The status of the timer interrupt. This bit is read-only */

/* Wait For Interrupt */
#define wfi() asm volatile("wfi" : : : "memory")

/* PSTATE and special purpose register access functions */
uint32_t raw_read_current_el();
uint32_t get_current_el();
uint32_t raw_read_daif();
void raw_write_daif(uint32_t daif);
void enable_debug_exceptions();
void enable_serror_exceptions();
void enable_irq();
void enable_fiq();
void disable_debug_exceptions();
void disable_serror_exceptions();
void disable_irq();
void disable_fiq();

/* ISR_EL1, Interrupt Status Register */
uint32_t raw_read_isr_el1();
uint64_t raw_read_rvbar_el1();
void raw_write_rvbar_el1(uint64_t rvbar_el1);
uint64_t raw_read_vbar_el1();
uint64_t raw_read_vbar_el2();
void raw_write_vbar_el1(uint64_t vbar_el1);

/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control register */
uint32_t raw_read_cntv_ctl();
void disable_cntv();
void enable_cntv();
/* CNTFRQ_EL0, Counter-timer Frequency register */
uint32_t raw_read_cntfrq_el0();
void raw_write_cntfrq_el0(uint32_t cntfrq_el0);
/* CNTVCT_EL0, Counter-timer Virtual Count register */
uint64_t raw_read_cntvct_el0();
/* CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register */
uint64_t raw_read_cntv_cval_el0();
void raw_write_cntv_cval_el0(uint64_t cntv_cval_el0);

uint64_t raw_read_scr_el3();
uint64_t raw_read_hcr_el2();

inline uint64_t read_spsr_el1() {
	uint64_t spsr_el1;
	asm volatile("mrs %0, SPSR_EL1\n\t" : "=r"(spsr_el1) : : "memory");
	return spsr_el1;
}

inline void write_spsr_el1(uint64_t spsr_el1) {
	asm volatile("msr SPSR_EL1, %0\n\t" : : "r"(spsr_el1) : "memory");
}

inline uint64_t read_spsr_el2() {
	uint64_t spsr_el2;
	asm volatile("mrs %0, SPSR_EL2\n\t" : "=r"(spsr_el2) : : "memory");
	return spsr_el2;
}

inline void write_spsr_el2(uint64_t spsr_el2) {
	asm volatile("msr SPSR_EL2, %0\n\t" : : "r"(spsr_el2) : "memory");
}

inline uint64_t read_icc_sre_el2() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_SRE_EL2\n\t" : "=r"(reg) : : "memory");
	return reg;
}

inline void write_icc_sre_el2(uint64_t reg) {
	asm volatile("msr ICC_SRE_EL2, %0\n\t" : : "r"(reg) : "memory");
}

#ifdef __cplusplus
}
#endif
#endif /*  _AARCH64_H   */
