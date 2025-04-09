#include "drivers/console.hh"
#include "drivers/cru_clksel.hh"
#include "drivers/cru_gate.hh"
#include "drivers/cru_reset.hh"
#include "drivers/grf.hh"
#include <algorithm>
#include <cstdint>
#include <cstdio>

#include "riscv/build/riscv_bin.h"

constexpr auto SYS_GRF_BASE = (0xFDC60000);
constexpr auto GRF_SOC_CON4 = (SYS_GRF_BASE + 0x510);
constexpr auto CRU_BASE = (0xFDD20000);
constexpr auto CRU_SOFTRST_CON26 = (CRU_BASE + 0x0468);
// constexpr auto SYSTEM_SRAM_BASE = (0xFDCC0000);
constexpr auto PMU_SRAM_BASE = (0xFFFF0000);
constexpr auto PMU_SRAM_BASE2 = (0xFDCD0000);

extern "C" int cruntimemain() {

	using namespace mdrivlib::RockchipPeriph;

	Console::init();

	CruGate::aclk_mcu_en::write(CruGate::cru_gate::clock_disable);
	CruGate::aclk_mcu_en::write(CruGate::cru_gate::clock_enable);

	mdrivlib::RockchipPeriph::Cru::Apll::fbdiv::write(0x44);
	mdrivlib::RockchipPeriph::Cru::Apll::postdiv1::write(0x1);
	mdrivlib::RockchipPeriph::Cru::Apll::bypass::clear();

	SysGrf::mcu_ahb2axi_d_buf_flush::set();
	SysGrf::mcu_ahb2axi_d_buf_flush::clear();
	SysGrf::mcu_ahb2axi_i_buf_flush::set();
	SysGrf::mcu_ahb2axi_i_buf_flush::clear();

	// This has an effect on MCU's GPIO toggle speed:
	// 32.4kHz with gpll_div_200m
	// 32.0kHz with gpll_div_150m
	// 30.1kHz with gpll_div_100m
	// 16.2kHz with xin_osc0
	CruClksel::aclk_bus_sel::write(CruClksel::aclk_bus_clock_mux::clk_gpll_div_200m);

	// This has an effect on MCU's GPIO toggle speed: ~30kHz with AHB, ~32kHz with AXI
	SysGrf::mcu_sel_axi::write(SysGrf::mcu_sel_axi_choice::use_axi);

	// CRU_GPLL_CON0 postdiv1 set to 1 (was 3)
	// 36.8kHz by setting postdiv1 to 1 (was 3) and fbdiv to 0x80
	// wr 0xfdd20040 0x7fff1080

	printf("Restting MCU...\n");
	Cru::areset_mcu::set();

	// copy mcu program to sysram
	// depending on the riscv program that was built, sometimes we crash here...
	// maybe it's because of the size of the included binary?
	printf("Copying binary to SRAM...\n");
	std::copy(riscv_bin, riscv_bin + riscv_bin_len, reinterpret_cast<uint8_t *>(PMU_SRAM_BASE2));

	// set mcu entrypoint
	// the entry point is aligned on a 64k boundry
	// thus 0xfdcc0000 is shifted to 0x0000fdcc
	// then we enable writing those bits with 0xffff0000
	printf("Setting MCU entry address...\n");
	// *reinterpret_cast<volatile uint32_t *>(GRF_SOC_CON4) = 0xffff'fdcc;
	*reinterpret_cast<volatile uint32_t *>(GRF_SOC_CON4) = 0xffff'fdcd;

	// printf("Press any key to start MCU...\n");
	// static_cast<void>(getchar());
	printf("Starting MCU\n");

	// run
	Cru::areset_mcu::clear();

	while (1) {
		Console::process();
	}
}
