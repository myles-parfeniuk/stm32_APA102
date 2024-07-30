#pragma once
#include "../APA102Animation.h"

class PulseAnimation: public APA102Animation {
public:
	PulseAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl, int16_t update_delay, uint16_t fade_time_ms, uint8_t red, uint8_t green, uint8_t blue);
	void set_color(uint8_t red, uint8_t green, uint8_t blue) override;
	bool next_frame() override;
	void start() override;
private:
	typedef struct fade_incrementer_t
	{
		float red;
		float green;
		float blue;
		float brightness;
	}fade_incrementer_t;


	void calculate_incrementers();
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	float actual_red;
	float actual_green;
	float actual_blue;
	float actual_brightness;
	uint16_t fade_time_ms;
	bool increment;
	fade_incrementer_t fade_inc;
};
