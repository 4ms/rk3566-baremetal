.cpu cortex-a55

.macro armv8_switch_to_el1_m, xreg1, xreg2

	/* Initialize Generic Timers */
	mrs	\xreg1, cnthctl_el2
	orr	\xreg1, \xreg1, #0x3	/* Enable EL1 access to timers */
	msr	cnthctl_el2, \xreg1
	msr	cntvoff_el2, xzr

	/* Initilize MPID/MPIDR registers */
	mrs	\xreg1, midr_el1
	mrs	\xreg2, mpidr_el1
	msr	vpidr_el2, \xreg1
	msr	vmpidr_el2, \xreg2

	/* Disable coprocessor traps */
	mov	\xreg1, #0x33ff
	msr	cptr_el2, \xreg1	/* Disable coprocessor traps to EL2 */
	msr	hstr_el2, xzr		/* Disable coprocessor traps to EL2 */
	mov	\xreg1, #3 << 20
	msr	cpacr_el1, \xreg1	/* Enable FP/SIMD at EL1 */

	/* Initialize HCR_EL2 */
	mov	\xreg1, #(1 << 31)		/* 64bit EL1 */
	msr	hcr_el2, \xreg1

	/* SCTLR_EL1 initialization
	 *
	 * setting RES1 bits (29,28,23,22,20,11) to 1
	 * and RES0 bits (31,30,27,21,17,13,10,6) +
	 * UCI,EE,EOE,WXN,nTWE,nTWI,UCT,DZE,I,UMA,SED,ITD,
	 * CP15BEN,SA0,SA,C,A,M to 0
	 */
	mov	\xreg1, #0x0800
	movk	\xreg1, #0x30d0, lsl #16
	msr	sctlr_el1, \xreg1

	/* Return to the EL1_SP1 mode from EL2 */
	mov	\xreg1, #0x3c4
	msr	spsr_el2, \xreg1	/* EL1_SP0 | D | A | I | F */
	adr	\xreg1, 3f
	msr	elr_el2, \xreg1
	eret
3:

	.endm

.section .start

.equ UART_THR, 0xfe660000

.global	_start
_start:					/* normally entered from armstub8 in EL2 after boot */
	ldr x0, =UART_THR   /* Print ABC */
	mov x1, #65
	str x1, [x0]
	mov x1, #66         
	str x1, [x0]
	mov x1, #67
	str x1, [x0]

/* Setup Stack */
	ldr	x0, =_irq_stack_end /* IRQ, FIQ and exception handler run in EL1h ?? */
	msr	sp_el1, x0		/* init their stack */

	ldr	x0, =_user_stack_end	/* main thread runs in EL1t and uses sp_el0 ?? */
	mov	sp, x0			/* init its stack */


	mrs x1, sctlr_el2
	orr x1, x1, #0x1000    /* I: bit 12 instruction cache */
	orr x1, x1, #0x0001    /* M: bit 1  MMU enable for EL1 and EL0  */
	orr x1, x1, #0x0004    /* C: bit 2  Cacheability control for data accesses at EL1 and EL0 */
	msr	sctlr_el2, x1

	mrs x1, cpacr_el1
	orr	x1, x1, #3 << 20        /* FPEN bits: don't trap FPU at EL1 or EL0 */
	orr	x1, x1, #3 << 22        /* Don't trap SIMD at EL1 or EL0*/
	msr	cpacr_el1, x1
	isb

	mrs x1, cptr_el2
	bic x0, x0, #(0x3 << 20)  // Ensure EL2 does not block FPU access
	msr cptr_el2, x0
	isb




	mrs x1, sctlr_el1
	orr x1, x1, #0x1000    /* I: bit 12 instruction cache */
	orr x1, x1, #0x0001    /* M: bit 1  MMU enable for EL1 and EL0  */
	orr x1, x1, #0x0004    /* C: bit 2  Cacheability control for data accesses at EL1 and EL0 */
	//bic x1, x1, #1 << 21   /* ?DN: enable fpu? */
	msr	sctlr_el1, x1

	ldr x0, =UART_THR
	mov x1, #68 //D
	str x1, [x0]

/*
 	adr	x1, 1f
 	msr	elr_el2, x1
 	eret
 1:
 */

	ldr x0, =UART_THR
	mov x1, #69 //E
	str x1, [x0]

	bl clear_bss
	bl init_statics

	b main


