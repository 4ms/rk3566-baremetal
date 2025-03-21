.cpu cortex-a55

.section .start

.equ UART_THR, 0xfe660000

.global	_start
_start:					/* normally entered in EL2 from u-boot */
	ldr x0, =UART_THR   /* Print ABC */
	mov x1, #65
	str x1, [x0]
	mov x1, #66		 
	str x1, [x0]
	mov x1, #67
	str x1, [x0]

// Setup Stacks
	ldr	x0, =_user_stack_end
	mov	sp, x0
//	ldr	x0, =_irq_stack_end
//	msr	sp_el1, x0

// Route IRQ, FIQ, and SError to EL2
	mrs x0, hcr_el2
	orr x0, x0, #(1<<5)
	orr x0, x0, #(1<<4)
	orr x0, x0, #(1<<3)
	msr hcr_el2, x0

// Setup vector table
	ldr	x1, =vectors
	msr	vbar_el2, x1

// Enable System Register access (ICC_*_EL2 system registers)
	mov x1, xzr
	orr x1, x1, #(1<<3)  // Enable EL1 to use ICC_SER_EL1
	orr x1, x1, #(1<<2)  // Disable IRQ bypass
	orr x1, x1, #(1<<1)  // Disable FIQ bypass
	orr x1, x1, #(1<<0)  // System Register Enable
	msr icc_sre_el2, x1


// Setup c runtime (bss, statics)

	bl clear_bss

	ldr x0, =UART_THR
	mov x1, #68 //D
	str x1, [x0]

	bl __libc_init_array

	ldr x0, =UART_THR
	mov x1, #69 //E
	str x1, [x0]


// Jump to main:
	b main

