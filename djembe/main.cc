#include "armv8-bare-metal/aarch64.h"
#include "djembe.hh"
#include "gpio.hh"
#include "mmu.h"
#include "serial.hh"
#include "uart.hh"

int main()
{
	using namespace RockchipPeriph;

	print('\n');
	auto el = get_current_el();
	print("Current EL: ");
	print((int)el);
	print('\n');

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));

	print("Enabling MMU:\n");
	enable_mmu();

	float x = 1.23f;
	float y = 2.45f;
	float z = std::pow(y, x);
	float a = z * 1000.f;
	int b = (int)a;
	print("2.45^1.23 = ");
	print(b / 1000);
	print('.');
	print((b / 100) % 10);
	print((b / 10) % 10);
	print(b % 10);
	print("\n");

	print("Playing djembe:\n");
	// 32ms to process 1000ms of sound
	MetaModule::DjembeCore dj;
	float in[48'000];
	for (unsigned i = 0; i < 48'000; i++) {
		if ((i % 12'000) == 0)
			in[i] = 1;
		else
			in[i] = 0;
	}
	float out[48'000];
	HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
	{
		for (unsigned i = 0; i < 48'000; i++) {
			dj.set_input(4, in[i]);
			dj.update();
			out[i] = dj.get_output(0);
		}
	}
	HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));

	print("Writing 1MB memory:\n");
	// takes 1ms
	HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
	{
		uint32_t *addr = reinterpret_cast<uint32_t *>(0x1100'0000);
		uint32_t *end = reinterpret_cast<uint32_t *>(0x1110'0000);
		uint32_t i = 0;
		while (addr < end) {
			*addr = out[i % 48'000];
			addr += 4;
		}
	}
	HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));

	unsigned i = 0;
	for (auto f : out) {
		int x = f * 255.f;
		if (f != 0.0f) {
			print(x);
			print(',');
			if ((++i % 20) == 0)
				print('\n');
		}
	}
	print('\n');

	print("Toggling GPIO0 C5:\n");

	uint32_t reps = 10;
	while (reps--) {
		// 8.3MHz
		HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
		HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));
	}

	print("Done\n");
	while (true)
		;
}
