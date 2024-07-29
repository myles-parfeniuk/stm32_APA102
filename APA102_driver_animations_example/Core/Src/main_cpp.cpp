/*
 * cpp_main.cpp
 *
 *	APA102 C++ driver, basic example.
 *
 *	This example lights up a single led at a time, shifting a red pixel over the length of the strip.
 *
 *	Originally compiled for an STM32L476RGT6 using the SPI1 perioheral with DMA; but, configuration can easily be changed using
 *	the 'APA102_driver_basic_example.ioc' file as a guideline for peripheral configuration.
 *	UART peripheral is used to print error messages but is not critical to functionality, comment out if desired.
 *
 *  Created on: Jul 26, 2024
 *      Author: myles
 */
#include "main_cpp.h"

#include <string.h>
#include "APA102Strip.h"
#include "APA102Animation.h"
#include "animations/ChaseAnimation.h"
#include "animations/FlickerAnimation.h"

static const constexpr uint16_t LED_COUNT = 60; //place your LED count here

volatile bool next_frame = false; //next frame flag triggered by timer ISR
APA102Animation *current_anim; //pointer to currently playing animation

void switch_animation(APA102Animation *new_anim);
void Animation_TIMPeriodElapsedCallback(TIM_HandleTypeDef *htim);

int cpp_main() {
	uint32_t start_tick = 0;
	APA102Strip strip(LED_COUNT, &hspi1); //create the led strip object

	//register time callback and create animations
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID,
			Animation_TIMPeriodElapsedCallback);

	//chase animation updating every 30ms, with 100 red, 10 green, and 0 blue intensity, each chase 7 LEDs in length, with 5 in total.
	ChaseAnimation chase_anim(strip, &htim2, 30, 100, 10, 0,
			APA102Strip::MAX_BRIGHTNESS, 7, 5);

	//flicker animation updating every 200ms, same color as above
	FlickerAnimation flicker_anim(strip, &htim2, 200, 100, 10, 0,
			APA102Strip::MAX_BRIGHTNESS / 4);

	//switch to and start the first animation
	switch_animation(&chase_anim);

	//set the start ticks for switching animations
	start_tick = HAL_GetTick();

	while (1) {
		if (next_frame) {
			next_frame = false;
			current_anim->next_frame();
		}

		if (HAL_GetTick() - start_tick >= 5000) {
			if (current_anim != &chase_anim)
				switch_animation(&chase_anim);
			else
				switch_animation(&flicker_anim);

			start_tick = HAL_GetTick();
		}

	}

	return 0;
}

void switch_animation(APA102Animation *new_anim)
{
	//set current animation
	current_anim = new_anim;
	//start the animation
	current_anim->start();


}

void Animation_TIMPeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		next_frame = true;
	}
}

