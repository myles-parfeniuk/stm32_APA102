#include "../APA102Animation.h"

class ChaseAnimation: public APA102Animation {
public:
	ChaseAnimation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl, int16_t update_delay, uint8_t red, uint8_t green, uint8_t blue,
			uint8_t brightness, uint16_t chase_length, uint16_t chase_count);
	void set_color(uint8_t red, uint8_t green, uint8_t blue);
	void set_brightness(uint8_t new_brightness);
	void next_frame() override;
	void start() override;
private:
	void construct_pixel_buffer();
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t brightness;
	uint16_t chase_length;
	uint16_t chase_count;
	uint16_t rotation_counter;
};
