/*
 * ChaseAnimation.cpp
 *
 *  Created on: Jul 28, 2024
 *      Author: myles
 */
#include "animations/ChaseAnimation.h"

ChaseAnimation::ChaseAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
		int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue,
		uint8_t brightness, uint16_t chase_length, uint16_t chase_count) :
		APA102Animation(strip, timer_hdl, update_delay), red(red), green(green), blue(
				blue), brightness(brightness), chase_length(chase_length), chase_count(
				chase_count) {

}

void ChaseAnimation::set_color(uint8_t red, uint8_t green, uint8_t blue) {
	stop();
	this->red = red;
	this->green = green;
	this->blue = blue;
	construct_pixel_buffer();
	HAL_TIM_Base_Start_IT(timer_hdl);
}

void ChaseAnimation::set_brightness(uint8_t new_brightness) {
	stop();
	brightness = new_brightness;
	construct_pixel_buffer();
	HAL_TIM_Base_Start_IT(timer_hdl);
}

bool ChaseAnimation::next_frame() {
	bool cycle_complete = false;
	std::span<apa102_pixel_t> pixels = strip.get_pixels();

	std::rotate(pixels.rbegin(), pixels.rbegin() + 1, pixels.rend());

	rotation_counter++;

	if (rotation_counter >= pixels.size())
	{
		rotation_counter = 0;
		cycle_complete = true;
	}

	strip.write_pixel_buffer();

	return cycle_complete;
}

void ChaseAnimation::start() {
	rotation_counter = 0;
	construct_pixel_buffer();
	set_update_delay(update_delay_ms);
	strip.write_pixel_buffer();
}

void ChaseAnimation::construct_pixel_buffer() {
	std::span<apa102_pixel_t> pixels = strip.get_pixels();
	uint16_t pixels_sz = pixels.size();
	int16_t whitespace = pixels_sz - (chase_count * chase_length);
	uint16_t spacing = whitespace / chase_count; //blank space between each chase
	uint16_t chase_buffer_sz;
	apa102_pixel_t *chase_buffer;
	uint16_t chase_idx = 0;

	//clear anything inside buffer
	strip.clear_pixel_buffer();

	if (whitespace < 0)
		return;

	//allocate memory for chase buffer
	chase_buffer_sz = chase_length + spacing;
	chase_buffer = new(std::nothrow) apa102_pixel_t[chase_buffer_sz]; //we don't want to throw an exception here, return nullptr on failure instead

	if(chase_buffer == nullptr)
		return; //memory allocation failure

	//build chase buffer
	//chase pixels
	for (int i = 0; i < chase_length; i++) {
		chase_buffer[chase_length - 1 - i] = apa102_pixel_t(red, green, blue,
				brightness / (i + 1)); //scale brightness
		chase_buffer[chase_length - 1 - i].color /= (i + 1); //scale color
	}

	//white space
	for (int i = chase_length; i < chase_buffer_sz; i++) {
		chase_buffer[i] = apa102_pixel_t(0, 0, 0, 0);
	}

	//build pixel buffer with chase buffer
	for (int i = 0; i < pixels_sz; i++) {
		pixels[i] = chase_buffer[chase_idx++];

		if (chase_idx >= chase_buffer_sz)
			chase_idx = 0;
	}

	std::rotate(pixels.rbegin(), pixels.rbegin() + rotation_counter,
	pixels.rend());
	//delete chase buffer
	delete[] chase_buffer;

}
