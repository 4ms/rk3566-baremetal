	.section .init

.equ UART_THR, 0xfe660000

	.globl	_start
_start:					/* normally entered from armstub8 in EL2 after boot */
	ldr x0, =UART_THR
	mov x1, #65
	str x1, [x0]
	mov x1, #66
	str x1, [x0]
	mov x1, #67
	str x1, [x0]
loop:
	mov x1, #68
	str x1, [x0]
	b loop
