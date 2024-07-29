#pragma once
#include "../APA102Animation.h"

class FlickerAnimation: public APA102Animation {
public:
	FlickerAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
			int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue,
			uint8_t brightness);
	void set_color(uint8_t red, uint8_t green, uint8_t blue) override;
	void set_brightness(uint8_t new_brightness);
	bool next_frame() override;
	void start() override;
private:
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t brightness;
	bool strip_on;
};
