#include "serial.hh"
#include "drivers/uart_base.hh"

// Blocking
char rk_getc() {
	while (!HW::UART2->lsr.data_ready)
		;
	auto c = HW::UART2->data;
	return c;
}

bool rk_hasc() {
	return HW::UART2->lsr.data_ready;
}

char rk_getc_raw() {
	return HW::UART2->data;
}

// Blocking
void rk_putc(char x) {
	while (!HW::UART2->lsr.tx_holding_reg_empty)
		;
	HW::UART2->data = x;
}
