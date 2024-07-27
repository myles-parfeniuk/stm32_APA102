/*
 * APA102Strip.cpp
 *
 *  Created on: Jul 26, 2024
 *      Author: myles
 */
#include "APA102Strip.h"

APA102Strip::APA102Strip(uint16_t led_count, SPI_HandleTypeDef *hdl_spi) :
		hdl_spi(hdl_spi), led_count(led_count) {
	//resize pixel vector according to LED count, with all pixel color intensities set to 0
	pixels.resize(led_count, (apa102_pixel_t ) { { 0, 0, 0 }, 0 });

	end_frame_count = calculate_end_frame_count(); //amount of end frames required such that data is shifted correctly
}

HAL_StatusTypeDef APA102Strip::write_pixel_buffer() {
	uint32_t frame_buffer_sz = led_count * 4 + (4 * 1) + (4 * end_frame_count); //[(LEDN frames + start frame + end frames)] bytes
	uint8_t *frame_buffer = new uint8_t[frame_buffer_sz]; //allocate memory for frame
	HAL_StatusTypeDef status = HAL_ERROR;

	if (frame_buffer == nullptr)
		return status; //failed to allocate memory

	//construct the frame
	construct_frame_buffer(frame_buffer, frame_buffer_sz);

	status = HAL_SPI_Transmit_DMA(hdl_spi, frame_buffer, frame_buffer_sz);

	delete[] frame_buffer; //ensure to free memory to avoid memory leaks

	return status;
}

void APA102Strip::clear_pixel_buffer() {
	std::fill(pixels.begin(), pixels.end(),
			(apa102_pixel_t ) { { 0, 0, 0 }, 0 });
}

bool APA102Strip::set_strip_color(apa102_rgb_color_t color,
		uint8_t brightness) {

	if (brightness > MAX_BRIGHTNESS)
		return false;

	std::fill(pixels.begin(), pixels.end(), (apa102_pixel_t ) { color,
					brightness });
	return true;
}

bool APA102Strip::set_pixel_color(uint16_t pixel, apa102_rgb_color_t color,
		uint8_t brightness) {
	if (pixel >= pixels.size())
		return false;

	if (brightness > MAX_BRIGHTNESS)
		return false;

	pixels[pixel] = { color, brightness };

	return true;
}

std::vector<apa102_pixel_t>& APA102Strip::get_frame_buffer() {
	return pixels;
}

void APA102Strip::set_led_count(uint16_t new_count) {
	clear_pixel_buffer();
	write_pixel_buffer();
	led_count = new_count;
	pixels.resize(led_count, { { 0, 0, 0 }, 0 });
}

int32_t APA102Strip::calculate_end_frame_count() {
	return (led_count + (END_FRAME_HALF_CLOCK_CYCLES - 1))
			/ END_FRAME_HALF_CLOCK_CYCLES;
}

void APA102Strip::construct_frame_buffer(uint8_t *frame_buffer,
		uint32_t length) {
	uint16_t pixel_index = 0;

	/*
	 frame structure:
	 start frame + ledn frames + end frames, each frame = 32 bits
	 building frame:
	 */

	// start frame:
	std::memset(frame_buffer, 0x00, 4);

	// color data frames
	for (uint16_t i = 4; i < length; i += 4) {
		pixel_index = i / 4 - 1;

		frame_buffer[i] = 0xE0 | pixels[pixel_index].brightness;
		frame_buffer[i + 1] = pixels[pixel_index].color.blue;
		frame_buffer[i + 2] = pixels[pixel_index].color.green;
		frame_buffer[i + 3] = pixels[pixel_index].color.red;
	}

	// end frame(s):
	std::memset(frame_buffer + (length - 4 * end_frame_count), 0xFF,
			4 * end_frame_count);
}
