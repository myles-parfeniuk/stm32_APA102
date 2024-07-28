/*
 * APA102Strip.cpp
 *
 *  Created on: Jul 26, 2024
 *      Author: myles
 */
#include "APA102Strip.h"

APA102Strip::APA102Strip(uint16_t led_count, SPI_HandleTypeDef *hdl_spi) :
		hdl_spi(hdl_spi), led_count(led_count) {

	initialize_frame_buffer(led_count);
}

HAL_StatusTypeDef APA102Strip::write_pixel_buffer() {
	uint32_t frame_buffer_sz = led_count * 4 + (4 * 1) + (4 * end_frame_count); //[(LEDN frames + start frame + end frames)] bytes
	uint8_t * frame_buffer = reinterpret_cast<uint8_t*>(pixels.data());
	HAL_StatusTypeDef status;

	status = HAL_SPI_Transmit_DMA(hdl_spi, frame_buffer, frame_buffer_sz);

	return status;
}

void APA102Strip::clear_pixel_buffer() {
	//only clear pixel data, not end or start frames
	std::fill(pixels.begin() + 1, pixels.end() - end_frame_count,
			apa102_pixel_t(0, 0, 0, 0));
}

bool APA102Strip::set_strip_color(uint8_t red, uint8_t green, uint8_t blue,
		uint8_t brightness) {

	if (brightness > MAX_BRIGHTNESS)
		return false;

	//only fill pixel data, not end or start frames
	std::fill(pixels.begin() + 1, pixels.end() - end_frame_count, apa102_pixel_t(red, green, blue, brightness));
	return true;
}

bool APA102Strip::set_pixel_color(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue,
		uint8_t brightness) {
	if (pixel >= pixels.size() - end_frame_count - 1)
		return false;

	if (brightness > MAX_BRIGHTNESS)
		return false;

	pixels[pixel + 1] = apa102_pixel_t(red, green, blue, brightness);

	return true;
}

apa102_pixel_t * APA102Strip::get_frame_buffer() {
	return pixels.data() + 1;
}

void APA102Strip::set_led_count(uint16_t new_count) {

	clear_pixel_buffer();
	write_pixel_buffer();
	led_count = new_count;
	initialize_frame_buffer(led_count);
}

void APA102Strip::initialize_frame_buffer(uint16_t led_count)
{
	pixels.clear(); //clear the pixel buffer

	/*
	 frame structure:
	 start frame + ledn frames + end frames, each frame = 32 bits
	 building frame:
	 */

	//resize pixel vector according to LED count, with all pixel color intensities set to 0
	pixels.resize(led_count, apa102_pixel_t(0, 0, 0, 0));
	//add start frame to start of pixels
	pixels.insert(pixels.begin() + 0, apa102_pixel_t(0x0000));
	//add end frames to end of pixels
	end_frame_count = calculate_end_frame_count(); //amount of end frames required such that data is shifted correctly
	for(int i = 0; i < end_frame_count; i++)
		pixels.push_back(apa102_pixel_t(0xFFFF));

}

int32_t APA102Strip::calculate_end_frame_count() {
	return (led_count + (END_FRAME_HALF_CLOCK_CYCLES - 1))
			/ END_FRAME_HALF_CLOCK_CYCLES;
}

