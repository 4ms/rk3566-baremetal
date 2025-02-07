.cpu cortex-a55

.section .init

.equ UART_THR, 0xfe660000

.global _start
_start:
/* Print ABC and then hang */
	ldr x0, =UART_THR
	mov x1, #65
	str x1, [x0]
	mov x1, #66
	str x1, [x0]
	mov x1, #67
	str x1, [x0]
	b .
