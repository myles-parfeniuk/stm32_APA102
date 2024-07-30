#pragma once
#include "../APA102Animation.h"
//comment out if not using real RNG peripheral
#include "../rng.h"

class CometAnimation: public APA102Animation {
public:
	CometAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl,
			int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue);
	void set_color(uint8_t red, uint8_t green, uint8_t blue) override;
	bool next_frame() override;
	void start() override;
private:
	typedef struct comet_t {
		std::vector<apa102_pixel_t> pixels;
		int16_t tail_loc;

		comet_t(int16_t tail_loc) :
			tail_loc(tail_loc) {
		}
	} comet_t;

	typedef struct meteroid_pixel_t {
		apa102_pixel_t pixel;
		int16_t loc;
		float fade_increment;
		float actual_brightness;

		meteroid_pixel_t(apa102_pixel_t pixel, int16_t loc, float fade_increment, float actual_brightness) :
				pixel(pixel), loc(loc), fade_increment(fade_increment), actual_brightness(actual_brightness) {

		}
	} meteroid_pixel_t;

	bool draw_comet_body(std::span<apa102_pixel_t>& pixels);
	void draw_meteroid_pixels(std::span<apa102_pixel_t>& pixels);
	void spawn_meteroid_pixel(std::span<apa102_pixel_t>& pixels);
	uint32_t generate_random_number();
	void construct_comet();
	bool process_fading_pixels(std::span<apa102_pixel_t>& pixels);

	uint8_t red;
	uint8_t green;
	uint8_t blue;
	comet_t comet;
	std::vector<meteroid_pixel_t> fading_pixels;

	static const constexpr uint16_t COMET_LENGTH = 12;
	static const constexpr float MIN_METEROID_FADE_TIME_MS = 20.0f;
	static const constexpr float MAX_METEROID_FADE_TIME_MS = 1000.0f;
};
