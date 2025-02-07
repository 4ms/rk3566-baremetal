#include <array>
#include <cstdint>

void print(int x);
void print(char x);
void print(const char *x);

const char *world = "World\n";
int number = 123456;
char X = 'X';

std::array<int, 4> arr{};			 // zero-init
std::array<int, 4> arr2{1, 2, 3, 4}; // value init

auto UART_THR = reinterpret_cast<volatile uint32_t *>(0xfe660000);

int main()
{
	print("Hello ");
	print(world);

	print(number);
	print('\n');
	print(-98765);
	print('\n');

	print(X);
	print('\n');

	for (auto a : arr)
		print(a);
	print('\n');

	for (auto a : arr2)
		print(a);
	print('\n');

	while (1)
		;
}

void print(int x)
{
	char buffer[32];
	char *p;

	if (x < 0) {
		print('-');
		x = -x;
	}
	p = buffer;

	do {
		*p++ = '0' + (x % 10);
		x /= 10;
	} while (x);

	do {
		print(*p);
		p--;
	} while (p > buffer);
}

void print(char x) { *UART_THR = 'x'; }

void print(const char *x)
{
	while (*x != '\0') {
		print(*x);
		x++;
	}
}
