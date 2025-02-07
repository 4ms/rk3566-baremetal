#include <cstdint>

namespace RockchipPeriph
{

struct Gpio {

	struct DataOutMask {
		uint16_t mask;
		uint16_t out;
	};
	DataOutMask data_L;
	DataOutMask data_H;

	struct DirMask {
		uint16_t mask;
		uint16_t dir_out;
	};
	DirMask dir_L;
	DirMask dir_H;

	struct EnableMask {
		uint16_t mask;
		uint16_t enable;
	};
	EnableMask intr_en_L;
	EnableMask intr_en_H;
	EnableMask intr_mask_L;
	EnableMask intr_mask_H;

	struct IntrType {
		uint16_t mask;
		uint16_t edge_sensitive; // Level 0, Edge 1
	};
	IntrType intr_type_L;
	IntrType intr_type_H;

	struct IntrPolarity {
		uint16_t mask;
		uint16_t active_high; // active low = 0, high = 1
	};
	IntrPolarity intr_pol_L;
	IntrPolarity intr_pol_H;

	EnableMask intr_bothedge_L;
	EnableMask intr_bothedge_H;

	EnableMask debounce_L;
	EnableMask debounce_H;

	EnableMask divide_enable_L;
	EnableMask divide_enable_H;

	uint32_t divide_control;

	uint32_t intr_status;
	uint32_t intr_rawstatus;

	struct EOIMask {
		uint16_t mask;
		uint16_t clear;
	};
	EOIMask intr_eoi_L;
	EOIMask intr_eoi_H;

	uint32_t ext_port;

	uint32_t ver_id;
};

namespace HW
{

static inline volatile RockchipPeriph::Gpio *const GPIO0 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfdd60000);
static inline volatile RockchipPeriph::Gpio *const GPIO1 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe740000);
static inline volatile RockchipPeriph::Gpio *const GPIO2 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe750000);
static inline volatile RockchipPeriph::Gpio *const GPIO3 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe760000);
static inline volatile RockchipPeriph::Gpio *const GPIO4 = reinterpret_cast<RockchipPeriph::Gpio *>(0xfe770000);

}; // namespace HW

} // namespace RockchipPeriph
