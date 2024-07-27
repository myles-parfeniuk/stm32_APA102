/*
 * APA102Strip.h
 *
 *	APA102 LED strip driver using stm32 HAL library.
 *
 *  Created on: Jul 26, 2024
 *      Author: myles
 */

#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
#include "spi.h"

typedef struct apa102_rgb_color_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	inline bool operator!=(const apa102_rgb_color_t &rhs) const {
		return (red != rhs.red) || (green != rhs.green) || (blue != rhs.blue);
	}

	inline bool operator==(const apa102_rgb_color_t &rhs) const {
		return (red == rhs.red) && (green == rhs.green) && (blue == rhs.blue);
	}

	inline apa102_rgb_color_t operator*(float factor) const {
		return {static_cast<uint8_t>(std::round((float) red * factor)), static_cast<uint8_t>(std::round((float) green * factor)),
			static_cast<uint8_t>(std::round((float) blue * factor))};
	}

	inline apa102_rgb_color_t operator/(uint8_t divisor) const {
		return {static_cast<uint8_t>(red / divisor), static_cast<uint8_t>(green / divisor), static_cast<uint8_t>(blue / divisor)};
	}

} rgb_color_t;

typedef struct apa102_pixel_t {
	rgb_color_t color;
	uint8_t brightness;
} apa102_pixel_t;

class APA102Strip {
public:
    static const constexpr uint8_t MAX_BRIGHTNESS = 31;

	APA102Strip(uint16_t led_count, SPI_HandleTypeDef *hdl_spi);
	HAL_StatusTypeDef write_pixel_buffer();
	void clear_pixel_buffer();
	bool set_strip_color(apa102_rgb_color_t color, uint8_t brightness);
	bool set_pixel_color(uint16_t pixel, apa102_rgb_color_t color, uint8_t brightness);
	std::vector<apa102_pixel_t>& get_frame_buffer();
	void set_led_count(uint16_t new_count);
private:
	static const constexpr uint32_t END_FRAME_HALF_CLOCK_CYCLES = 64;

	SPI_HandleTypeDef *hdl_spi;
	uint16_t led_count;
	uint16_t end_frame_count;
	std::vector<apa102_pixel_t> pixels;

	void construct_frame_buffer(uint8_t *frame_buffer, uint32_t length);
	int32_t calculate_end_frame_count();
};
