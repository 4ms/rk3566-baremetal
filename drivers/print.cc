#include "serial.hh"

void print(int x)
{
	char buffer[32];
	char *p;

	if (x < 0) {
		rk_putc('-');
		x = -x;
	}
	p = buffer;

	do {
		*p++ = '0' + (x % 10);
		x /= 10;
	} while (x);

	do {
		rk_putc(*--p);
	} while (p > buffer);
}

void print(char x) { rk_putc(x); }

void print(const char *str)
{
	while (*str)
		rk_putc(*str++);
}
