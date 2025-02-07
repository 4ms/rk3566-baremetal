.cpu cortex-a55

.section .init

.equ UART_THR, 0xfe660000

.globl	_start
_start:					/* normally entered from armstub8 in EL2 after boot */
/* Print ABC */
	ldr x0, =UART_THR
	mov x1, #65
	str x1, [x0]
	mov x1, #66
	str x1, [x0]
	mov x1, #67
	str x1, [x0]

/* Clear BSS */
	ldr x1, =_bss_start
	ldr x2, =_bss_end

clear_bss_loop:
	str  xzr, [x1], #8
	cmp  x1, x2
	bls  clear_bss_loop

/* Setup Stack */
	ldr	x0, =_irq_stack_end /* IRQ, FIQ and exception handler run in EL1h */
	msr	sp_el1, x0		/* init their stack */


1:	ldr	x0, =_user_stack_end	/* main thread runs in EL1t and uses sp_el0 */
	mov	sp, x0			/* init its stack */

	b main


