#include "serial.hh"
#include "uart.hh"

#include <array>

const char *world = "World\n";
int number = 123456;
char X = 'X';

std::array<int, 4> arr{};			 // zero-init
std::array<int, 4> arr2{1, 2, 3, 4}; // value init

int init_value(int x);
std::array<int, 4> arr3{init_value(0), init_value(1), init_value(2), init_value(3)};

extern "C" int cruntimemain()
{
	print('\n');
	print("Hello ");
	print(world);

	print("Should be '123456': ");
	print(number);
	print('\n');

	print("Should be '-789': ");
	print(-789);
	print('\n');

	print("Should be 'X': ");
	print(X);
	print('\n');

	print("Should be '0000': ");
	for (auto a : arr)
		print(a);
	print('\n');

	print("Should be '1234': ");
	for (auto a : arr2)
		print(a);
	print('\n');

	print("Should be '55667788': ");
	for (auto a : arr3)
		print(a);
	print('\n');

	print("Press a key to see it echoed back CAPITAL and lowercase:\n");
	while (1) {
		auto c = getc();

		if (c >= 'A' && c <= 'Z') {
			print(char(c));
			print(char(c - 'A' + 'a'));
		} else if (c >= 'a' && c <= 'z') {
			print(char(c));
			print(char(c - 'a' + 'A'));
		}
	}
}
