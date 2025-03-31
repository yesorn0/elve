/*
 * water.c
 *
 *  Created on: Mar 24, 2025
 *      Author: USER
 */
#include "water.h"



// 일정 수준이상 물 감지되면 1 반환
uint8_t WATER_Check()
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	uint16_t water = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	printf("water : %d\n", water);

	if(water > 1000)
		return 1;
	else
		return 0;
}
