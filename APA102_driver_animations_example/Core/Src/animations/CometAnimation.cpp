/*
 * CometAnimation.cpp
 *
 *  Created on: Jul 29, 2024
 *      Author: myles
 */
#include "animations/CometAnimation.h"

CometAnimation::CometAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
		int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue) :
		APA102Animation(strip, timer_hdl, update_delay), red(red), green(green), blue(
				blue),  comet(comet_t(0)) {

}

void CometAnimation::set_color(uint8_t red, uint8_t green, uint8_t blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;

	stop();
	if (fading_pixels.size() != 0)
		for (uint32_t i = 0; i < fading_pixels.size(); i++) {
			fading_pixels[i].pixel.color.red = red;
			fading_pixels[i].pixel.color.green = green;
			fading_pixels[i].pixel.color.blue = blue;
		}

	for (uint32_t i = 0; i < comet.pixels.size(); i++) {
		comet.pixels[i].color.red = red;
		comet.pixels[i].color.green = green;
		comet.pixels[i].color.blue = blue;
	}
	HAL_TIM_Base_Start_IT(timer_hdl);
}

bool CometAnimation::next_frame() {
	strip.clear_pixel_buffer();
	std::span<apa102_pixel_t> pixels = strip.get_pixels();
	bool comet_off_tape = true; //the comet body is no longer visible on the LED tape (moved off frame)
	bool cycle_complete = false;

	comet_off_tape = draw_comet_body(pixels);

	spawn_meteroid_pixel(pixels);

	draw_meteroid_pixels(pixels);

	strip.write_pixel_buffer();

	if (comet_off_tape && process_fading_pixels(pixels)) {
		stop();
		comet.tail_loc = 0;
		fading_pixels.clear();
		HAL_Delay(500);
		HAL_TIM_Base_Start_IT(timer_hdl);
		cycle_complete = true;
	}

	return cycle_complete;

}

void CometAnimation::start() {
	construct_comet();
	set_update_delay(update_delay_ms);
}

bool CometAnimation::draw_comet_body(std::span<apa102_pixel_t> &pixels) {
	uint16_t pixels_sz = pixels.size();
	uint16_t comet_idx = 0;

	for (int i = 0; i < COMET_LENGTH; i++) {
		comet_idx = i + comet.tail_loc;

		//only draw if visible on tape
		if (comet_idx >= 0 && comet_idx < pixels_sz)
			pixels[comet_idx] = comet.pixels[i];
	}

	comet.tail_loc++;

	if (comet.tail_loc > pixels_sz)
		return true;

	return false;

}

void CometAnimation::draw_meteroid_pixels(std::span<apa102_pixel_t> &pixels) {
	if (fading_pixels.size() != 0) {
		for (const auto &fading_pixel : fading_pixels)
			pixels[fading_pixel.loc] = fading_pixel.pixel;
	}

}

void CometAnimation::spawn_meteroid_pixel(std::span<apa102_pixel_t> &pixels) {
	static const constexpr int16_t COMET_SPAWN_RATE = 5; // 5 == 1 / 5 chance, 4 == 1 / 4 chance, etc...
	float fade_time_ms = 0.0f;
	float fade_frames = 0.0f;
	float fade_incrementer = 0.0f; //how much the brightness level will be decreased for each next_frame() call

	//only spawn if in frame
	if (comet.tail_loc >= 0 && comet.tail_loc < (int16_t) pixels.size())
		if (generate_random_number() % COMET_SPAWN_RATE == 0) {
			fade_time_ms = generate_random_number()
					% ((uint32_t) MAX_METEROID_FADE_TIME_MS
							- (uint32_t) MIN_METEROID_FADE_TIME_MS + 1);
			fade_frames = fade_time_ms / update_delay_ms + 1;
			fade_incrementer = (float) APA102Strip::MAX_BRIGHTNESS / fade_frames;
			fading_pixels.push_back(
					meteroid_pixel_t(
							apa102_pixel_t(red, green, blue, APA102Strip::MAX_BRIGHTNESS),
							comet.tail_loc, fade_incrementer,
							(float) APA102Strip::MAX_BRIGHTNESS));
		}
}

uint32_t CometAnimation::generate_random_number() {
	uint32_t rand_number = 0;

	//comment in if not using real rng peripheral:
	//rand_number = std::rand();

	//comment out if not using real rng peripheral:
	HAL_RNG_GenerateRandomNumber(&hrng, &rand_number);

	return rand_number;

}

void CometAnimation::construct_comet() {
	fading_pixels.clear();
	comet = comet_t(0 - COMET_LENGTH); //create comet starting at 0 - length pixel (not visible on tape)
	comet.pixels.resize(COMET_LENGTH,
			apa102_pixel_t(red, green, blue, APA102Strip::MAX_BRIGHTNESS));
}

bool CometAnimation::process_fading_pixels(std::span<apa102_pixel_t> &pixels) {
	bool complete = false;

	if (fading_pixels.size() != 0) {
		complete = true;

		for (uint32_t i = 0; i < fading_pixels.size(); i++) {
			fading_pixels[i].actual_brightness -=
					fading_pixels[i].fade_increment;

			if (fading_pixels[i].actual_brightness < 0)
				fading_pixels[i].actual_brightness = 0;
			else
				complete = false;

			fading_pixels[i].pixel = apa102_pixel_t(red, green, blue,
					std::round(fading_pixels[i].actual_brightness));
		}

	}

	//the very rare case that the comet body passed through the tape without any fading pixels being spawned
	if ((int) fading_pixels.size() == 0
			&& comet.tail_loc >= (int) pixels.size())
		complete = true;

	return complete;
}
