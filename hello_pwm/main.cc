#include "drivers/grf.hh"
#include "drivers/pwm.hh"
#include <algorithm>
#include <cstdio>

extern "C" int cruntimemain() {

	// enable pwm output
	HW::PMU->gpio0_b_h.write(Rockchip::GPIO0B_IOMUX_H_SEL_7::PWM0_M0);
	HW::PWM0->chan[0].period = 255;
	HW::PWM0->chan[0].duty = 128;
	HW::PWM0->chan[0].control = 0x03;

	printf("\n\nPWM OUTPUT!!\n");
	printf("Connect scope to pin 13\n");
	printf("Press a number to adjust duty cycle\n");
	while (1) {
		auto c = getchar();
		c -= '0';
		c = std::clamp(c, 0, 9);
		HW::PWM0->chan[0].duty = (c / 10.f * 255);
	}
}
