#include "drivers/grf.hh"
#include "drivers/pmu.hh"
#include "drivers/pwm.hh"
#include <algorithm>
#include <cstdio>

extern "C" int cruntimemain() {

	using namespace mdrivlib::RockchipPeriph;

	PmuGrf::pwm2_iomux_sel::write(PmuGrf::choice_pwm2_iomux::m1);
	GrfIofunc::pwm11_iomux_sel::write(GrfIofunc::choice_iomux2::m1);

	// enable pwm output
	// pin 13
	HW::PMU->gpio0_b_h.write(Rockchip::GPIO0B_IOMUX_H_SEL_7::PWM0_M0);

	// pin 32
	HW::SYS->gpio4_c_l.write(Rockchip::GPIO4C_IOMUX_L_SEL_0::PWM11_M1);

	// HW::PMU->gpio0_b_h.write(Rockchip::GPIO4C_IOMUX_L_SEL_0::PWM11_M1);

	for (unsigned i = 0; i < 4; i++) {
		{
			auto &chan = HW::PWM0->chan[i];
			chan.period = 255;
			chan.duty = 128;
			chan.control = 0x03;
		}
		{
			auto &chan = HW::PWM1->chan[i];
			chan.period = 255;
			chan.duty = 128;
			chan.control = 0x03;
		}
		{
			auto &chan = HW::PWM2->chan[i];
			chan.period = 255;
			chan.duty = 128;
			chan.control = 0x03;
		}
		{
			auto &chan = HW::PWM3->chan[i];
			chan.period = 255;
			chan.duty = 128;
			chan.control = 0x03;
		}
	}

	printf("\n\nPWM OUTPUT!!\n");
	printf("Connect scope to pin 13\n");
	printf("Press a number to adjust duty cycle\n");
	while (1) {
		auto c = getchar();
		c -= '0';
		c = std::clamp(c, 0, 9);
		for (unsigned i = 0; i < 4; i++) {
			HW::PWM0->chan[i].duty = (c / 10.f * 255);
			HW::PWM1->chan[i].duty = (c / 10.f * 255);
			HW::PWM2->chan[i].duty = (c / 10.f * 255);
			HW::PWM3->chan[i].duty = (c / 10.f * 255);
		}
	}
}
