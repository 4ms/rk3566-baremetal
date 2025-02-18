#include <array>
#include <cstdint>

namespace RockchipPeriph
{

struct PWM {
	struct Chan {
		const uint32_t count;
		uint32_t period;
		uint32_t duty;
		uint32_t control;
	};

	Chan chan[4];
	uint32_t intsts;
	uint32_t int_en;
	// TODO: finish pwm registers
};

} // namespace RockchipPeriph

namespace HW
{
static inline volatile RockchipPeriph::PWM *const PWM0 = reinterpret_cast<RockchipPeriph::PWM *>(0xFDD70000);
static inline volatile RockchipPeriph::PWM *const PWM1 = reinterpret_cast<RockchipPeriph::PWM *>(0xFE6E0000);
static inline volatile RockchipPeriph::PWM *const PWM2 = reinterpret_cast<RockchipPeriph::PWM *>(0xFE6F0000);
static inline volatile RockchipPeriph::PWM *const PWM3 = reinterpret_cast<RockchipPeriph::PWM *>(0xFE700000);

}; // namespace HW
