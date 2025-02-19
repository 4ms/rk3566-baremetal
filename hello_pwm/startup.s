.cpu cortex-a55

.section .start

.equ UART_THR, 0xfe660000

.global	_start
_start:					/* normally entered from armstub8 in EL2 after boot */
/* Print ABC */
	ldr x0, =UART_THR
	mov x1, #65
	str x1, [x0]
	mov x1, #66
	str x1, [x0]
	mov x1, #67
	str x1, [x0]

/* Setup Stack */
	ldr	x0, =_irq_stack_end /* IRQ, FIQ and exception handler run in EL1h */
	msr	sp_el1, x0		/* init their stack */

	ldr	x0, =_user_stack_end	/* main thread runs in EL1t and uses sp_el0 */
	mov	sp, x0			/* init its stack */


	ldr x0, =UART_THR
	mov x1, #68
	str x1, [x0]

	bl clear_bss
	bl init_statics

	b cruntimemain


