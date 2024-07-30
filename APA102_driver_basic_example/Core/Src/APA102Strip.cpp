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
	uint8_t *frame_buffer_ptr = reinterpret_cast<uint8_t*>(frame_buffer.data()); //cast the frame buffer vector to a uint8_t array
	HAL_StatusTypeDef status;
	HAL_SPI_StateTypeDef spi_state;

	//write frame buffer to DMA and transmit over SPI
	status = HAL_SPI_Transmit_DMA(hdl_spi, frame_buffer_ptr, frame_buffer_sz);

	//wait until all data is out of the DMA before proceeding
	do
	{
		spi_state =  HAL_SPI_GetState(hdl_spi);
		HAL_Delay(1);
	}while(spi_state != HAL_SPI_STATE_READY);

	return status;
}

void APA102Strip::clear_pixel_buffer() {
	//only clear pixel data, not end or start frames
	std::fill(pixels.begin(), pixels.end(), apa102_pixel_t(0, 0, 0, 0));
}

bool APA102Strip::set_strip_color(uint8_t red, uint8_t green, uint8_t blue,
		uint8_t brightness) {

	if (brightness > MAX_BRIGHTNESS)
		return false;

	//only fill pixel data, not end or start frames
	std::fill(pixels.begin(), pixels.end(),
			apa102_pixel_t(red, green, blue, brightness));
	return true;
}

bool APA102Strip::set_pixel_color(uint16_t pixel, uint8_t red, uint8_t green,
		uint8_t blue, uint8_t brightness) {
	if (pixel >= frame_buffer.size() - end_frame_count - 1)
		return false;

	if (brightness > MAX_BRIGHTNESS)
		return false;

	pixels[pixel] = apa102_pixel_t(red, green, blue, brightness);

	return true;
}

std::span<apa102_pixel_t> APA102Strip::get_pixels() {
	return pixels;
}

void APA102Strip::set_led_count(uint16_t new_count) {

	clear_pixel_buffer();
	write_pixel_buffer();
	led_count = new_count;
	initialize_frame_buffer(led_count);
}

void APA102Strip::initialize_frame_buffer(uint16_t led_count) {
	frame_buffer.clear(); //clear the pixel buffer

	/*
	 frame structure:
	 start frame + ledn frames + end frames, each frame = 32 bits
	 building frame:
	 */

	//resize pixel vector according to LED count, with all pixel color intensities set to 0
	frame_buffer.resize(led_count, apa102_pixel_t(0, 0, 0, 0));
	//add start frame to start of frame_buffer
	frame_buffer.insert(frame_buffer.begin() + 0, apa102_pixel_t(0x0000));
	//add end frames to end of frame_buffer
	end_frame_count = calculate_end_frame_count(); //amount of end frames required such that data is shifted correctly
	for (int i = 0; i < end_frame_count; i++)
		frame_buffer.push_back(apa102_pixel_t(0xFFFF));

	pixels = std::span<apa102_pixel_t>(frame_buffer.data() + 1,
			frame_buffer.size() - 1 - end_frame_count);

}

int32_t APA102Strip::calculate_end_frame_count() {
	return (led_count + (END_FRAME_HALF_CLOCK_CYCLES - 1))
			/ END_FRAME_HALF_CLOCK_CYCLES;
}

