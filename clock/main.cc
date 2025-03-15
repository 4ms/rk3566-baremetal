#include "djembe.hh"
#include "drivers/aarch64_system_reg.hh"
#include "drivers/console.hh"
#include "drivers/gpio.hh"
#include "mmu.h"
#include <cstdio>

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(pin, "pin", CONSOLE_INT_ARG_DEF(duration, "number of cycles"));
static void pin_command_handler(const pin_args_t *args) {
	for (int i = 0; i < args->duration; i++) {
		HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
		HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);
	}
}

float *gout;

CONSOLE_COMMAND_DEF(play, "play", CONSOLE_INT_ARG_DEF(dummy, "dummy"));
static void play_command_handler(const play_args_t *args) {
	printf("Playing djembe patch to create 1 second of samples:\n");

	float in[48'000];
	float out[48'000];

	gout = out;

	MetaModule::DjembeCore dj;
	for (unsigned i = 0; i < 48'000; i++) {
		if ((i % 12'000) == 0)
			in[i] = 1;
		else
			in[i] = 0;
	}

	// 32ms to process 1000ms of sound
	HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
	{
		for (unsigned i = 0; i < 48'000; i++) {
			dj.set_input(4, in[i]);
			dj.update();
			out[i] = dj.get_output(0);
		}
	}
	HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);

	printf("Done:\n");
}

CONSOLE_COMMAND_DEF(wrmem, "wrmem", CONSOLE_INT_ARG_DEF(dummy, "dummy"));
static void wrmem_command_handler(const wrmem_args_t *args) {
	printf("Writing 1MB memory:\n");
	HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
	{
		uint32_t *addr = reinterpret_cast<uint32_t *>(0x1100'0000);
		uint32_t *end = reinterpret_cast<uint32_t *>(0x1110'0000);
		uint32_t i = 0;
		while (addr < end) {
			*addr = gout[i % 48'000];
			addr += 4;
		}
	}
	HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);
	printf("Done:\n");
}

int main() {
	printf("AAA\n");

	console_command_register(pin);
	console_command_register(play);
	console_command_register(wrmem);

	using namespace RockchipPeriph;

	// Set up GPIO0_C5 as output
	HW::GPIO0->dir_H = Gpio::masked_set_bit(Gpio::C(5));

	printf("Enabling MMU:\n");
	enable_mmu();

	Console::init();

	printf("Ready\n");

	while (true) {
		Console::process();
		asm("nop");
	}
}
