/*
 * ChaseAnimation.cpp
 *
 *  Created on: Jul 28, 2024
 *      Author: myles
 */
#include "animations/FlickerAnimation.h"

FlickerAnimation::FlickerAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
		int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue,
		uint8_t brightness) :
		APA102Animation(strip, timer_hdl, update_delay), red(red), green(green), blue(
				blue), brightness(brightness) {

}

void FlickerAnimation::set_color(uint8_t red, uint8_t green, uint8_t blue) {
	HAL_TIM_Base_Stop_IT(timer_hdl);
	this->red = red;
	this->green = green;
	this->blue = blue;

	if(strip_on)
	{
		strip.set_strip_color(red, green, blue, brightness);
		strip.write_pixel_buffer();
	}

	HAL_TIM_Base_Start_IT(timer_hdl);
}

void FlickerAnimation::set_brightness(uint8_t new_brightness) {
	HAL_TIM_Base_Stop_IT(timer_hdl);
	brightness = new_brightness;
	HAL_TIM_Base_Start_IT(timer_hdl);
}

bool FlickerAnimation::next_frame() {
	bool cycle_complete = false;

	if(strip_on)
	{
		strip.set_strip_color(red, green, blue, brightness);
		strip.write_pixel_buffer();
	}
	else
	{
		strip.clear_pixel_buffer();
		strip.write_pixel_buffer();
		cycle_complete = true;
	}

	strip_on = !strip_on;

	return cycle_complete;
}

void FlickerAnimation::start() {
	set_update_delay(update_delay_ms);
	strip.set_strip_color(red, green, blue, brightness);
	strip_on = true;
	strip.write_pixel_buffer();
}
