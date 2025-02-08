#include "aarch64.h"
#include "gpio.hh"
#include "memory_layout.h"
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

	print("Writing 1MB memory:\n");
	// takes 1ms
	HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
	{
		uint32_t *addr = reinterpret_cast<uint32_t *>(0x1100'0000);
		uint32_t *end = reinterpret_cast<uint32_t *>(0x1110'0000);
		uint32_t i = 0;
		while (addr < end) {
			*addr = i;
			i *= 0x1234'567AF;
			addr += 4;
		}
	}
	HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));

	print("Writing 1MB coherent memory\n");
	HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
	{
		uint32_t *addr = reinterpret_cast<uint32_t *>(MEMORY_START_DMA);
		uint32_t *end = reinterpret_cast<uint32_t *>(MEMORY_START_DMA + 0x10'0000);
		uint32_t i = 0;
		while (addr < end) {
			*addr = i;
			i *= 0xAF12'9876;
			addr += 4;
		}
	}
	HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));

	print("Toggling GPIO0 C5:\n");
	uint32_t reps = 100;
	while (reps--) {
		// 8.3MHz
		HW::GPIO0->data_H = Gpio::masked_set_bit(Gpio::C(5));
		HW::GPIO0->data_H = Gpio::masked_clr_bit(Gpio::C(5));
	}

	print("Done\n");
	while (true)
		;
}
