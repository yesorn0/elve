/*
 * photo.c
 *
 *  Created on: Mar 25, 2025
 *      Author: USER
 */


#include "photo.h"

//가려지면 1
uint8_t Get_3cm_photo_data(void)
{
	return HAL_GPIO_ReadPin( GPIOB, PHOTO_3CM_Pin);
}
uint8_t Get_9cm_photo_data(void)
{
	return HAL_GPIO_ReadPin( GPIOB, PHOTO_9CM_Pin);
}
uint8_t Get_15cm_photo_data(void)
{
	return HAL_GPIO_ReadPin( GPIOB, PHOTO_15CM_Pin);
}
