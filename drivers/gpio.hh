#include <cstdint>

namespace RockchipPeriph
{

struct Gpio {
	static uint32_t masked_set_bit(uint8_t bit) { return (1 << bit) | (1 << (bit + 16)); }
	static uint32_t masked_clr_bit(uint8_t bit) { return (0 << bit) | (1 << (bit + 16)); }

	static uint32_t masked(uint16_t mask, uint16_t bits) { return (mask << 16) | bits; }

	// L
	static uint32_t A(uint32_t x) { return x << 0; }
	static uint32_t B(uint32_t x) { return x << 8; }
	// H
	static uint32_t C(uint32_t x) { return x << 0; }
	static uint32_t D(uint32_t x) { return x << 8; }

	// 0 = Output low, 1 = Output high
	uint32_t data_L;
	uint32_t data_H;

	// 0 = Input, 1 = Output
	uint32_t dir_L;
	uint32_t dir_H;

	uint32_t intr_en_L;
	uint32_t intr_en_H;
	uint32_t intr_mask_L;
	uint32_t intr_mask_H;

	// Level 0, Edge 1
	uint32_t intr_type_L;
	uint32_t intr_type_H;

	// active low = 0, active high = 1
	uint32_t intr_pol_L;
	uint32_t intr_pol_H;

	uint32_t intr_bothedge_L;
	uint32_t intr_bothedge_H;

	uint32_t debounce_L;
	uint32_t debounce_H;

	uint32_t divide_enable_L;
	uint32_t divide_enable_H;

	uint32_t divide_control;

	uint32_t intr_status;
	uint32_t intr_rawstatus;

	struct EOIMask {
		uint16_t mask;
		uint16_t clear;
	};
	uint32_t intr_eoi_L;
	uint32_t intr_eoi_H;

	uint32_t ext_port;

	uint32_t ver_id;
};

} // namespace RockchipPeriph

namespace HW
{

static inline volatile RockchipPeriph::Gpio *const GPIO0 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfdd60000);
static inline volatile RockchipPeriph::Gpio *const GPIO1 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe740000);
static inline volatile RockchipPeriph::Gpio *const GPIO2 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe750000);
static inline volatile RockchipPeriph::Gpio *const GPIO3 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe760000);
static inline volatile RockchipPeriph::Gpio *const GPIO4 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe770000);

}; // namespace HW
