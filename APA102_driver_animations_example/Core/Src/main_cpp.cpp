/*
 * cpp_main.cpp
 *
 *	APA102 C++ driver, animations example.
 *
 *	This example loops different animations which are implemented as child classes.
 *	The animation and color is changed every ANIMATION_DISPLAY_TIME_MS. It loops through 3 different animations and 5 different colors.
 *
 *	Originally compiled for an STM32L476RGT6 using the SPI1 peripheral with DMA, and TIM2 to time animations; but, configuration can easily be changed using
 *	the 'APA102_driver_animations_example.ioc' file as a guideline for peripheral configuration.
 *
 *	- Adjust LED_COUNT to the correct amount for your tape
 *	- Modify ANIMATION_DISPLAY_TIME_MS if you want the animations to display for a shorter/longer amount of time
 *	- The CometAnimation uses the hardware RNG peripheral, if you don't have/want to use it, see generate_random_number() inside CometAnimation.cpp
 *
 *
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
#include "animations/CometAnimation.h"

//adjust LED_COUNT and display time
static const constexpr uint16_t LED_COUNT = 60; //place your LED count here
static const constexpr uint16_t ANIMATION_DISPLAY_TIME_MS = 15000; //display animation 15 seconds before switching to next

//only adjust these if adding animations
static const constexpr uint8_t TOTAL_ANIM = 3; //total amount of animations to loop through
static const constexpr uint8_t MAX_COLORS = 5; //total amount of colors to loop through

volatile bool next_frame = false; //next frame flag triggered by timer ISR
APA102Animation *current_anim; //pointer to currently playing animation

void switch_animation(APA102Animation *new_anim);
void Animation_TIMPeriodElapsedCallback(TIM_HandleTypeDef *htim);

int cpp_main() {
	uint32_t start_tick = 0;
	uint8_t anim_idx = 1;
	uint8_t color_idx = 0;
	bool cycle_complete;

	//color buffer
	apa102_rgb_color_t colors[MAX_COLORS] = { { 0, 10, 255 }, { 255, 0, 0 }, {
			0, 50, 255 }, { 20, 0, 255 }, { 0, 255, 0 } }; //if using color struct directly, (b, g, r);

	APA102Strip strip(LED_COUNT, &hspi1); //create the led strip object

	//register time callback and create animations
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID,
			Animation_TIMPeriodElapsedCallback);

	//chase animation updating every 30ms, with 100 red, 10 green, and 0 blue intensity, each chase 7 LEDs in length, with 5 in total.
	ChaseAnimation chase_anim(strip, &htim2, 30, 100, 10, 0,
			APA102Strip::MAX_BRIGHTNESS, 7, 5);

	//flicker animation updating every 200ms, same color as above
	FlickerAnimation flicker_anim(strip, &htim2, 200, 100, 10, 0,
			APA102Strip::MAX_BRIGHTNESS);

	//comet animation updating every 10ms, with 255 red intensity and 10 green intensity
	CometAnimation comet_anim(strip, &htim2, 10, 255, 10, 0);

	//current anim buffer
	APA102Animation *anims[TOTAL_ANIM] = { &comet_anim, &chase_anim,
			&flicker_anim };

	//switch to and start the first animation
	switch_animation(&comet_anim);

	//set the start ticks for switching animations
	start_tick = HAL_GetTick();

	while (1) {
		//wait for TIM2 (frame timer) local interrupt flag
		if (next_frame) {
			next_frame = false; //de-assert flag
			cycle_complete = current_anim->next_frame(); //switch to next frame
		}

		//switch to next animation and color when ANIMATION_DISPLAY_TIME_MS and animation is inbetween cycles (as to not end it abbruptly)
		if (HAL_GetTick() - start_tick >= ANIMATION_DISPLAY_TIME_MS
				&& cycle_complete) {
			//switch animation
			switch_animation(anims[anim_idx]);
			//set color
			current_anim->set_color(colors[color_idx].red,
					colors[color_idx].green, colors[color_idx].blue);

			//handle indexes
			anim_idx++;
			color_idx++;

			if (anim_idx >= TOTAL_ANIM)
				anim_idx = 0;

			if (color_idx >= MAX_COLORS)
				color_idx = 0;

			//reset animation switch timer
			start_tick = HAL_GetTick();
		}

	}

	return 0;
}

void switch_animation(APA102Animation *new_anim) {
	//set current animation
	current_anim = new_anim;
	//start the animation
	current_anim->start();

}

void Animation_TIMPeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		next_frame = true; //assert local interrupt flag
	}
}

