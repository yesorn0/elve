#include "ultrasonic.h"

// 지상 (Floor)
static uint16_t IC_Value1_F = 0;
static uint16_t IC_Value2_F = 0;
static uint16_t echoTime_F = 0;
static uint8_t captureFlag_F = 0;
uint8_t distance_F = 0;

// 지하 (Basement)
static uint16_t IC_Value1_B = 0;
static uint16_t IC_Value2_B = 0;
static uint16_t echoTime_B = 0;
static uint8_t captureFlag_B = 0;
uint8_t distance_B = 0;



//void HCSR04_Trigger(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t TIM_IT_CCx)
//{
//		uint32_t prev = __HAL_TIM_SET_COUNTER(&htim3, 0);
//
//		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);
//
//		if(delay_us(&prev, 1) == 1)
//			HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 1);
//
//		if(delay_us(&prev, 11) == 1)
//    	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);
//
//    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CCx);
//}

void HCSR04_Trigger(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t TIM_IT_CCx)
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);
	delay_us(1);

	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, 0);

	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CCx);
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	// channel 1이 active되어있으면
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(captureFlag_F == 0)
		{
			IC_Value1_F = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
			captureFlag_F = 1;

			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if(captureFlag_F == 1) // 한번 capture 했으면
		{
			IC_Value2_F = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

			if(IC_Value2_F > IC_Value1_F)
			{
				echoTime_F = IC_Value2_F - IC_Value1_F;
			}
			else if(IC_Value1_F > IC_Value2_F)
			{
				echoTime_F = (0xffff - IC_Value1_F) + IC_Value2_F;
			}

			distance_F = echoTime_F / 58; // 거리 계산식은 datasheet에
			captureFlag_F = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC1);
		}
	}

	else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		if(captureFlag_B == 0) // 아직 capture 안했으면
		{
			IC_Value1_B = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2);
			captureFlag_B = 1;   // capture 했다고 flag 세움

			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if(captureFlag_B == 1)
		{
			IC_Value2_B = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2);

			if(IC_Value2_B > IC_Value1_B)
			{
				echoTime_B = IC_Value2_B - IC_Value1_B;
			}
			else if(IC_Value1_B > IC_Value2_B)
			{
				echoTime_B = (0xffff - IC_Value1_B) + IC_Value2_B;
			}

			distance_B = echoTime_B / 58; // 거리 계산식은 datasheet에
			captureFlag_B = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC2);
		}
	}
}
