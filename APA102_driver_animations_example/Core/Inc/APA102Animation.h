#pragma once

#include "APA102Strip.h"

#include "tim.h"

class APA102Animation
{
    public:
		APA102Animation(APA102Strip& strip, TIM_HandleTypeDef *timer_hdl, int16_t update_delay);
        void stop();
        void set_update_delay(int16_t delay_ms);
        virtual void start() = 0;
        virtual bool next_frame() = 0;
        virtual void set_color(uint8_t red, uint8_t green, uint8_t blue) = 0;

    protected:
        APA102Strip& strip;
    	TIM_HandleTypeDef *timer_hdl;
    	int16_t update_delay_ms;
        static const constexpr int32_t TIM_TICK_FREQ = 20000000UL;
};
