#include "djembe.hh"
#include "drivers/aarch64_system_reg.hh"
#include "drivers/gpio.hh"
#include "mmu.h"
#include <cstdio>

int main() {
	using namespace RockchipPeriph;

	printf("\n");

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));

	printf("Enabling MMU:\n");
	enable_mmu();

	float x = 1.23f;
	float y = 2.45f;
	float z = std::pow(y, x);
	printf("2.45^1.23 = %g\n", z);

	printf("Playing djembe:\n");
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

	printf("Writing 1MB memory:\n");
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
			printf("%d,", x);
			if ((++i % 20) == 0)
				printf("\n");
		}
	}
	printf("\n");

	printf("Toggling GPIO0 C5:\n");

	uint32_t reps = 10;
	while (reps--) {
		// 8.3MHz
		HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
		HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));
	}

	printf("Done\n");
	while (true)
		;
}
