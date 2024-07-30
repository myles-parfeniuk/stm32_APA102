#include "APA102Animation.h"

APA102Animation::APA102Animation(APA102Strip &strip, TIM_HandleTypeDef *timer_hdl, int16_t update_delay_ms) :
		strip(strip), timer_hdl(timer_hdl), update_delay_ms(update_delay_ms) {

}

void APA102Animation::stop()
{
	HAL_TIM_Base_Stop_IT(timer_hdl);
}

void APA102Animation::set_update_delay(int16_t delay_ms) {
	uint32_t timer_period = 0;

	update_delay_ms = delay_ms;
	timer_period = (uint32_t)std::round(
			((float) update_delay_ms / 1000.0f) * (float) TIM_TICK_FREQ); //calculate amount of ticks based off frequency

	HAL_TIM_Base_Stop_IT(timer_hdl);
	htim2.Init.Period = timer_period;
	HAL_TIM_Base_Init(timer_hdl); //re-initialize timer with new time
	HAL_TIM_Base_Start_IT(timer_hdl);
}
