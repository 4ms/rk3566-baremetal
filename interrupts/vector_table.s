// Tags used in dump to tell us what kind of exception happened
.equ CUR_EL_SYNC_SP0, 1
.equ CUR_EL_SERR_SP0, 2
.equ CUR_EL_SYNC_SPX, 3
.equ CUR_EL_SERR_SPX, 4
.equ LOWER_EL_SYNC, 5
.equ LOWER_EL_SERR, 6
.equ LOWER_EL_A32_SYNC, 7
.equ LOWER_EL_A32_SERR, 8


.equ UART_THR, 0xfe660000

.macro handle_exception exc_type
	// TODO:
	mov	x21, #(\exc_type)
	mrs	x22, esr_el2
	stp	x21, x22, [sp, #-16]!
	//////
	b .
.endm


.section .vector_table
	.align 11

	.globl vectors
vectors:
	/*
	 * Current EL with SP0
	 */
	.balign 128
	handle_exception CUR_EL_SYNC_SP0
	.balign 128
	b	unhandled_int
	.balign 128
	b	unhandled_int
	.balign 128
	handle_exception CUR_EL_SERR_SP0

	/*
	 * Current EL with SPx
	 */
	.balign 128
	handle_exception CUR_EL_SYNC_SPX
	.balign 128
	b	handle_irq
	.balign 128
	b	unhandled_int
	.balign 128
	handle_exception CUR_EL_SERR_SPX

	/*
	 * Lower EL using AArch64
	 */
	.balign 128
	handle_exception LOWER_EL_SYNC
	.balign 128
	b	unhandled_int
	.balign 128
	b	unhandled_int
	.balign 128
	handle_exception LOWER_EL_SERR

	/*
	 * Lower EL using AArch32
	 */
	.balign 128
	handle_exception LOWER_EL_A32_SYNC

	.balign 128
	b	unhandled_int

	.balign 128
	b	unhandled_int

	.balign 128
	handle_exception LOWER_EL_A32_SERR

/*
 * Must save corruptible registers and non-corruptible registers expected to be
 * used, x0 and lr expected to be already saved on the stack
 */
.macro	push_interrupt_context
/*
 * Push x0-x21,lr on to the stack, need 19-21 because they're modified without
 * obeying PCS
 */
	stp x0,		lr,		[sp, #-0x10]! // Do we use lr?
	stp x0,		x1,		[sp, #-0x10]! 
	stp x2,		x3,		[sp, #-0x10]!
	stp x4,		x5,		[sp, #-0x10]!
	stp x6,		x7,		[sp, #-0x10]!
	stp x8,		x9,		[sp, #-0x10]!
	stp x10,	x11,	[sp, #-0x10]!
	stp x12,	x13,	[sp, #-0x10]!
	stp x14,	x15,	[sp, #-0x10]!
	stp x16,	x17,	[sp, #-0x10]!
	stp x18,	x19,	[sp, #-0x10]!
	stp x20,	x21,	[sp, #-0x10]!
/*
 * Push q0-q31 on to the stack, need everything because parts of every register
 * are volatile/corruptible
 */
	stp q0,		q1,		[sp, #-0x20]!
	stp q2,		q3,		[sp, #-0x20]!
	stp q4,		q5,		[sp, #-0x20]!
	stp q6,		q7,		[sp, #-0x20]!
	stp q8,		q9,		[sp, #-0x20]!
	stp q10,	q11,	[sp, #-0x20]!
	stp q12,	q13,	[sp, #-0x20]!
	stp q14,	q15,	[sp, #-0x20]!
	stp q16,	q17,	[sp, #-0x20]!
	stp q18,	q19,	[sp, #-0x20]!
	stp q20,	q21,	[sp, #-0x20]!
	stp q22,	q23,	[sp, #-0x20]!
	stp q24,	q25,	[sp, #-0x20]!
	stp q26,	q27,	[sp, #-0x20]!
	stp q28,	q29,	[sp, #-0x20]!
	stp q30,	q31,	[sp, #-0x20]!
/* Push exception LR for PC and spsr */
	mrs x0, 	elr_el2
	mrs x1, 	spsr_el2
	stp x0,		x1,	[sp, #-0x10]!
/* Push fpcr and fpsr */
	mrs x0, fpsr
	mrs x1, fpcr
	stp x0,	x1,	[sp, #-0x10]!
.endm

/* Must match inverse order of .push_interrupt_context */
.macro pop_interrupt_context
/* Pop fpcr and fpsr */
	ldp x0,	x1,	[sp], #0x10
/* Restore fpcr and fpsr */
	msr fpcr, x1
	msr fpsr, x0
/* Pop pc and spsr */
	ldp x0,		x1,	[sp], #0x10
/* Restore exception LR for PC and spsr */
	msr spsr_el2, x1
	msr elr_el2, x0
/* Pop q0-q31 */
	ldp q30,	q31,	[sp], #0x20
	ldp q28,	q29,	[sp], #0x20
	ldp q26,	q27,	[sp], #0x20
	ldp q24,	q25,	[sp], #0x20
	ldp q22,	q23,	[sp], #0x20
	ldp q20,	q21,	[sp], #0x20
	ldp q18,	q19,	[sp], #0x20
	ldp q16,	q17,	[sp], #0x20
	ldp q14,	q15,	[sp], #0x20
	ldp q12,	q13,	[sp], #0x20
	ldp q10,	q11,	[sp], #0x20
	ldp q8,		q9,		[sp], #0x20
	ldp q6,		q7,		[sp], #0x20
	ldp q4,		q5,		[sp], #0x20
	ldp q2,		q3,		[sp], #0x20
	ldp q0,		q1,		[sp], #0x20
/* Pop x0-x21, lr */
	ldp x20,	x21,	[sp], #0x10
	ldp x18,	x19,	[sp], #0x10
	ldp x16,	x17,	[sp], #0x10
	ldp x14,	x15,	[sp], #0x10
	ldp x12,	x13,	[sp], #0x10
	ldp x10,	x11,	[sp], #0x10
	ldp x8,		x9,		[sp], #0x10
	ldp x6,		x7,		[sp], #0x10
	ldp x4,		x5,		[sp], #0x10
	ldp x2,		x3,		[sp], #0x10
	ldp x0,		x1,		[sp], #0x10 
	ldp x0,		lr,		[sp], #0x10

/* Must clear reservations here to ensure consistency with atomic operations */
	clrex
.endm

handle_irq:

	push_interrupt_context

	mrs x0, icc_iar1_el1

	// Skip if spurious
	cmp x0, #1020
	bhi exit_irq_handler

	str x0,	[sp, #-0x08]! // Store the INTID

	// enable interupts: 
	msr DAIFClr, #(1<<1)

	bl ISRHandler

	// disable interupts: 
	msr DAIFSet, #(1<<1)

	ldr x0, [sp], #0x08 	// Pop INTID

	msr icc_eoir1_el1, x0 	// Acknowledge

exit_irq_handler:

	pop_interrupt_context

	eret

unhandled_int:
	b .

