/*
 * cpp_main.cpp
 *
 *	APA102 C++ driver, basic example.
 *
 *	This example lights up a single led at a time, shifting a red pixel over the length of the strip.
 *
 *	Originally compiled for an STM32L476RGT6 using the SPI1 peripheral with DMA; but, configuration can easily be changed using
 *	the 'APA102_driver_basic_example.ioc' file as a guideline for peripheral configuration.
 *
 *	- Adjust LED_COUNT to the correct amount for your tape
 *
 *  Created on: Jul 26, 2024
 *      Author: myles
 */
#include "main_cpp.h"

#include <string.h>
#include "APA102Strip.h"

static const constexpr uint16_t LED_COUNT = 60; //place your LED count here

int cpp_main()
{
	uint16_t pixel = 0; //current pixel to draw

	APA102Strip strip(LED_COUNT, &hspi1); //create the led strip object

	while(1)
	{
		strip.clear_pixel_buffer();
		strip.set_pixel_color(pixel, 50, 0, 0, APA102Strip::MAX_BRIGHTNESS); //set the currently chosen pixel to 50/255 red intensity, max brightness
		strip.write_pixel_buffer();

		//delay for 50ms before shifting to next pixel
		HAL_Delay(50);
		pixel++;

		//reset current pixel back to 0
		if(pixel >= LED_COUNT)
			pixel = 0;

	}

	return 0;
}
