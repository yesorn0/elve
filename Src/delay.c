#include "delay.h"

// delay_us를 만드는 함수
//uint8_t delay_us(uint32_t *prev, uint32_t interval)
//{
//	uint32_t curr = __HAL_TIM_GET_COUNTER(&htim3);
//
//	if(curr - *prev >= interval)
//	{
//		*prev = curr;
//		return 1;
//	}
//	else
//		return 0;
//}

void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while((__HAL_TIM_GET_COUNTER(&htim11)) < us);
}


uint8_t delay_ms(uint32_t *prev, uint32_t interval)
{
	uint32_t curr = HAL_GetTick();

	if(curr - *prev >= interval)
	{
		*prev = curr;
		return 1;
	}
	else
		return 0;
}
