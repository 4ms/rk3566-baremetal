#include "djembe.hh"
#include "drivers/console.hh"
#include "drivers/gpio.hh"
#include <cstdio>

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(gpio,
					"Toggle GPIOX_YZ n times",
					CONSOLE_INT_ARG_DEF(GPIOnum, "GPIO# 0-4"),
					CONSOLE_STR_ARG_DEF(port_letter, "Port Letter A-D"),
					CONSOLE_INT_ARG_DEF(pin, "Pin 0-7"),
					CONSOLE_INT_ARG_DEF(pulses, "number of pulses"));
static void gpio_command_handler(const gpio_args_t *args) {
	using namespace RockchipPeriph;

	auto gp = args->GPIOnum == 0 ? HW::GPIO0 :
			  args->GPIOnum == 1 ? HW::GPIO1 :
			  args->GPIOnum == 2 ? HW::GPIO2 :
			  args->GPIOnum == 3 ? HW::GPIO3 :
			  args->GPIOnum == 4 ? HW::GPIO4 :
								   HW::GPIO0;
	auto port = args->port_letter[0] == 'A' ? Gpio::Port::A :
				args->port_letter[0] == 'B' ? Gpio::Port::B :
				args->port_letter[0] == 'C' ? Gpio::Port::C :
				args->port_letter[0] == 'D' ? Gpio::Port::D :
				args->port_letter[0] == 'a' ? Gpio::Port::A :
				args->port_letter[0] == 'b' ? Gpio::Port::B :
				args->port_letter[0] == 'c' ? Gpio::Port::C :
				args->port_letter[0] == 'd' ? Gpio::Port::D :
											  Gpio::Port::A;
	gp->dir_output(port, args->pin);

	for (int i = 0; i < args->pulses; i++) {
		gp->high(port, args->pin);
		gp->low(port, args->pin);
	}
}

float *gout;

CONSOLE_COMMAND_DEF(play, "play", CONSOLE_INT_ARG_DEF(dummy, "dummy"));
static void play_command_handler(const play_args_t *args) {
	printf("Playing djembe patch to create 1 second of samples:\n");
	printf("Measure the pulse width of GPIO0_C5 (cm3io pin 31) to see how long it took\n");

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

	// process 1000ms of sound
	HW::GPIO0->high(RockchipPeriph::Gpio::Port::C, 5);
	{
		for (unsigned i = 0; i < 48'000; i++) {
			dj.set_input(4, in[i]);
			dj.update();
			out[i] = dj.get_output(0);
		}
	}
	HW::GPIO0->low(RockchipPeriph::Gpio::Port::C, 5);

	printf("Done\n");
}

CONSOLE_COMMAND_DEF(wrmem, "wrmem", CONSOLE_INT_ARG_DEF(dummy, "dummy"));
static void wrmem_command_handler(const wrmem_args_t *args) {
	printf("Writing 1MB memory:\n");
	printf("Measure the pulse width of GPIO0_C5 (cm3io pin 31) to see how long it took\n");
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
	printf("Done\n");
}

int main() {
	console_command_register(play);
	console_command_register(wrmem);
	console_command_register(gpio);

	using namespace RockchipPeriph;

	Console::init();
	printf("\n");
	printf("Common pins to toggle 99 times:\n");
	printf("gpio 0 C 5 99\n");
	printf("gpio 4 B 1 99\n");
	printf("gpio 4 B 4 99\n");
	printf("gpio 3 D 0 99\n");
	printf("Ready\n");

	while (true) {
		Console::process();
		asm("nop");
	}
}
