#include "drivers/console.hh"
#include <algorithm>
#include <cstdint>
#include <cstdio>

#include "riscv/build/riscv_bin.h"

constexpr auto SYS_GRF_BASE = (0xFDC60000);
constexpr auto GRF_SOC_CON4 = (SYS_GRF_BASE + 0x510);
constexpr auto CRU_BASE = (0xFDD20000);
constexpr auto CRU_SOFTRST_CON26 = (CRU_BASE + 0x0468);
constexpr auto SYSTEM_SRAM_BASE = (0xFDCC0000);

extern "C" int cruntimemain() {

	Console::init();

	// reset mcu
	*reinterpret_cast<volatile uint32_t *>(CRU_SOFTRST_CON26) = 0xffff0000 | (1 << 10);

	// copy mcu program to sysram
	// depending on the riscv program that was built, sometimes we crash here...
	// maybe it's because of the size of the included binary?
	std::copy(riscv_bin, riscv_bin + riscv_bin_len, reinterpret_cast<uint8_t *>(SYSTEM_SRAM_BASE));
	// set mcu entrypoint
	// the entry point is aligned on a 64k boundry
	// thus 0xfdcc0000 is shifted to 0x0000fdcc
	// then we enable writing those bits with 0xffff0000
	*reinterpret_cast<volatile uint32_t *>(GRF_SOC_CON4) = 0xffff'fdcc;

	printf("Press any key to start MCU...\n");
	static_cast<void>(getchar());
	printf("Starting MCU\n");

	// run
	*reinterpret_cast<volatile uint32_t *>(CRU_SOFTRST_CON26) = 0xffff0000;

	while (1) {
		Console::process();
	}
}
