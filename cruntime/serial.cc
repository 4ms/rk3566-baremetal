#include "uart.hh"

// Blocking
char getc()
{
	while (!HW::UART2->lsr.data_ready)
		;
	auto c = HW::UART2->data;
	return c;
}

// Blocking
void putc(char x)
{
	while (!HW::UART2->lsr.tx_holding_reg_empty)
		;
	HW::UART2->data = x;
}

void puts(const char *str)
{
	while (*str != '\0') {
		putc(*str);
		str++;
	}
}

void print(int x)
{
	char buffer[32];
	char *p;

	if (x < 0) {
		putc('-');
		x = -x;
	}
	p = buffer;

	do {
		*p++ = '0' + (x % 10);
		x /= 10;
	} while (x);

	do {
		putc(*--p);
	} while (p > buffer);
}

void print(char x) { putc(x); }

void print(const char *str) { puts(str); }
