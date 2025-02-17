/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/*  Exception vector                                                    */
/*                                                                      */
/************************************************************************/

.equ AARCH64_EXC_SYNC_SP0, 0x1 /* Synchronous */
.equ AARCH64_EXC_IRQ_SP0, 0x2  /* IRQ/vIRQ */
.equ AARCH64_EXC_FIQ_SP0, 0x3  /* FIQ/vFIQ */
.equ AARCH64_EXC_SERR_SP0, 0x4 /* SError/vSError */
/* Current EL with SPx */
.equ AARCH64_EXC_SYNC_SPX, 0x11
.equ AARCH64_EXC_IRQ_SPX, 0x12
.equ AARCH64_EXC_FIQ_SPX, 0x13
.equ AARCH64_EXC_SERR_SPX, 0x14
/* Lower EL using AArch64 */
.equ AARCH64_EXC_SYNC_AARCH64, 0x21
.equ AARCH64_EXC_IRQ_AARCH64, 0x22
.equ AARCH64_EXC_FIQ_AARCH64, 0x23
.equ AARCH64_EXC_SERR_AARCH64, 0x24
/* Lower EL using AArch32 */
.equ AARCH64_EXC_SYNC_AARCH32, 0x31
.equ AARCH64_EXC_IRQ_AARCH32, 0x32
.equ AARCH64_EXC_FIQ_AARCH32, 0x33
.equ AARCH64_EXC_SERR_AARCH32, 0x34

/*
 * exception_frame offset definitions
 */
.equ EXC_FRAME_SIZE, 288	 /* sizeof(struct _exception_frame) */
.equ EXC_EXC_TYPE_OFFSET, 0	 /* __asm_offsetof(struct _exception_frame, exc_type) */
.equ EXC_EXC_ESR_OFFSET, 8	 /* __asm_offsetof(struct _exception_frame, exc_esr) */
.equ EXC_EXC_SP_OFFSET, 16	 /* __asm_offsetof(struct _exception_frame, exc_sp) */
.equ EXC_EXC_ELR_OFFSET, 24	 /* __asm_offsetof(struct _exception_frame, exc_elr) */
.equ EXC_EXC_SPSR_OFFSET, 32 /* __asm_offsetof(struct _exception_frame, exc_spsr) */

.section ".text.boot"

.macro build_trapframe exc_type
	/*
	 * store generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	/*
	 * Store (spsr, x0)
	 */
	mrs	x21, spsr_el2
	stp	x21, x0, [sp, #-16]!
	/*
	 * Allocate a room for sp_el0 and store elr
	 */
	mrs	x21, elr_el2
	stp	xzr, x21, [sp, #-16]!
	/*
	 * store exception type and esr
	 */
	mov	x21, #(\exc_type)
	mrs	x22, esr_el2
	stp	x21, x22, [sp, #-16]!
.endm

.macro store_traped_sp
	mov x21, sp
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro call_common_trap_handler
	mov	x0, sp
	bl common_trap_handler
.endm

.macro store_nested_sp
	mov	x21, sp 	
	add	x21, x21, #EXC_FRAME_SIZE
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro restore_traped_sp
	ldr	x21, [sp, #EXC_EXC_SP_OFFSET]
	mov sp, x21
.endm

.macro restore_trapframe

	/*
	 * Drop exception type, esr, 
	 */
	add	sp, sp, #16
	/*
	 * Drop exception stack pointer and restore elr_el1
	 */
	ldp	x21, x22, [sp], #16	
	//msr	elr_el2, x22
	mov x0, x22

	/*
	 * Retore spsr and x0
	 */
	ldp	x21, x0, [sp], #16
	msr	spsr_el2, x21

	/*
	 * Restore generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16

	ret
.endm
	
	/*
	 * Exception vectors.
	 */
	.align 11
	.globl vectors
vectors:
	/*
	 * Current EL with SP0
	 */
	.align 7
	b	_curr_el_sp0_sync		/* Synchronous */
	.align 7
	b	_curr_el_sp0_irq		/* IRQ/vIRQ */
	.align 7
	b	_curr_el_sp0_fiq		/* FIQ/vFIQ */
	.align 7
	b	_curr_el_sp0_serror		/* SError/vSError */

	/*
	 * Current EL with SPx
	 */
	.align 7
	b	_curr_el_spx_sync		/* Synchronous */
	.align 7
	b	_curr_el_spx_irq		/* IRQ/vIRQ */
	.align 7
	b	_curr_el_spx_fiq		/* FIQ/vFIQ */
	.align 7
	b	_curr_el_spx_serror		/* SError/vSError */

	/*
	 * Lower EL using AArch64
	 */
	.align 7
	b	_lower_el_aarch64_sync
	.align 7
	b	_lower_el_aarch64_irq
	.align 7
	b	_lower_el_aarch64_fiq
	.align 7
	b	_lower_el_aarch64_serror

	/*
	 * Lower EL using AArch32
	 */
	.align 7
	b	_lower_el_aarch32_sync
	.align 7
	b	_lower_el_aarch32_irq
	.align 7
	b	_lower_el_aarch32_fiq
	.align 7
	b	_lower_el_aarch32_serror


	.align 2
_curr_el_sp0_sync:
	build_trapframe AARCH64_EXC_SYNC_SP0
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe

	.align 2
_curr_el_sp0_irq:
	build_trapframe AARCH64_EXC_IRQ_SP0
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe

	.align 2
_curr_el_sp0_fiq:
	build_trapframe AARCH64_EXC_FIQ_SP0
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe

	.align 2
_curr_el_sp0_serror:
	build_trapframe AARCH64_EXC_SERR_SP0
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe


	.align 2
_curr_el_spx_sync:
	build_trapframe AARCH64_EXC_SYNC_SPX
	store_nested_sp
	call_common_trap_handler
	restore_trapframe

	.align 2
_curr_el_spx_irq:
	build_trapframe AARCH64_EXC_IRQ_SPX	
	store_nested_sp
	call_common_trap_handler
	restore_trapframe

	.align 2
_curr_el_spx_fiq:
	build_trapframe AARCH64_EXC_FIQ_SPX
	store_nested_sp
	call_common_trap_handler	
	restore_trapframe

	.align 2
_curr_el_spx_serror:
	build_trapframe AARCH64_EXC_SERR_SPX	
	store_nested_sp
	call_common_trap_handler
	restore_trapframe


	.align 2
_lower_el_aarch64_sync:
	build_trapframe AARCH64_EXC_SYNC_AARCH64
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch64_irq:
	build_trapframe AARCH64_EXC_IRQ_AARCH64	
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch64_fiq:
	build_trapframe AARCH64_EXC_FIQ_AARCH64
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch64_serror:
	build_trapframe AARCH64_EXC_SERR_AARCH64	
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe


	.align 2
_lower_el_aarch32_sync:
	build_trapframe AARCH64_EXC_SYNC_AARCH32	
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch32_irq:
	build_trapframe AARCH64_EXC_IRQ_AARCH32
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch32_fiq:
	build_trapframe AARCH64_EXC_FIQ_AARCH32
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe
	
	.align 2
_lower_el_aarch32_serror:
	build_trapframe AARCH64_EXC_SERR_AARCH32
	store_traped_sp
	call_common_trap_handler
	restore_traped_sp
	restore_trapframe

