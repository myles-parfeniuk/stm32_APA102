/*
 * ChaseAnimation.cpp
 *
 *  Created on: Jul 28, 2024
 *      Author: myles
 */
#include "animations/PulseAnimation.h"

PulseAnimation::PulseAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
		int16_t update_delay, uint16_t fade_time_ms, uint8_t red, uint8_t green,
		uint8_t blue) :
		APA102Animation(strip, timer_hdl, update_delay), red(red), green(green), blue(
				blue), actual_red(0.0f), actual_green(0.0f), actual_blue(0.0f), actual_brightness(
				0.0f), fade_time_ms(fade_time_ms), increment(true) {

}

void PulseAnimation::set_color(uint8_t red, uint8_t green, uint8_t blue) {
	HAL_TIM_Base_Stop_IT(timer_hdl);
	this->red = red;
	this->green = green;
	this->blue = blue;

	calculate_incrementers();

	HAL_TIM_Base_Start_IT(timer_hdl);
}

void PulseAnimation::calculate_incrementers() {
	float fade_frames = (float) fade_time_ms / (float) update_delay_ms;

	fade_inc.red = (float) red / fade_frames;
	fade_inc.green = (float) green / fade_frames;
	fade_inc.blue = (float) blue / fade_frames;
	fade_inc.brightness = (float) APA102Strip::MAX_BRIGHTNESS / fade_frames;

	actual_red = 0;
	actual_brightness = 0;
	actual_blue = 0;
	actual_green = 0;

	increment = true;

}

bool PulseAnimation::next_frame() {
	bool cycle_complete = false;

	if (increment) {
		actual_red += fade_inc.red;
		if (actual_red > static_cast<float>(red))
			actual_red = static_cast<float>(red);

		actual_green += fade_inc.green;
		if (actual_green > static_cast<float>(green))
			actual_green = static_cast<float>(green);

		actual_blue += fade_inc.blue;
		if (actual_blue > static_cast<float>(blue))
			actual_blue = static_cast<float>(blue);

		actual_brightness += fade_inc.brightness;
		if (actual_brightness > static_cast<float>(APA102Strip::MAX_BRIGHTNESS))
			actual_brightness = static_cast<float>(APA102Strip::MAX_BRIGHTNESS);

		if (actual_red == static_cast<float>(red)
				&& actual_green == static_cast<float>(green)
				&& actual_blue == static_cast<float>(blue)
				&& actual_brightness
						== static_cast<float>(APA102Strip::MAX_BRIGHTNESS)) {
			increment = !increment;
		}

	} else {
		actual_red -= fade_inc.red;
		if (actual_red < 0)
			actual_red = 0;

		actual_green -= fade_inc.green;
		if (actual_green < 0)
			actual_green = 0;

		actual_blue -= fade_inc.blue;
		if (actual_blue < 0)
			actual_blue = 0;

		actual_brightness -= fade_inc.brightness;
		if (actual_brightness < 0)
			actual_brightness = 0;

		if (actual_red == 0 && actual_green == 0 && actual_blue == 0
				&& actual_brightness == 0) {
			increment = !increment;
			cycle_complete = true;
		}
	}

	strip.clear_pixel_buffer();

	strip.set_strip_color(static_cast<uint8_t>(std::round(actual_red)),
			static_cast<uint8_t>(std::round(actual_green)),
			static_cast<uint8_t>(std::round(actual_blue)),
			static_cast<uint8_t>(std::round(actual_brightness)));

	strip.write_pixel_buffer();

	return cycle_complete;
}

void PulseAnimation::start() {
	calculate_incrementers();
	set_update_delay(update_delay_ms);}
