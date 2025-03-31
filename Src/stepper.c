/*
 * stepper.c
 *
 *  Created on: Mar 26, 2025
 *      Author: USER
 */

#include "stepper.h"


void stepMotor(uint8_t step)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, HALF_STEP_SEQ[step][0]);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, HALF_STEP_SEQ[step][1]);
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, HALF_STEP_SEQ[step][2]);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, HALF_STEP_SEQ[step][3]);
}
void rotateSteps(uint16_t steps, uint8_t direction)
{
	for(uint16_t i = 0; i < steps; i++)
	{
		//스텝 방향에 따라서 패턴 설정
		uint8_t step;
		if(direction == DIR_CCW)
		{
			step = i % 8; //반시계
		}
		else
		{
			step = 7 - (i % 8); //시계
		}
		stepMotor(step);
		delay_us(1000); //모터속도 조절
	}
}

void rotateDegrees(uint16_t degrees, uint8_t direction)
{
	//각도에 해당하는 스텝수 계산
	uint16_t steps = (uint16_t)(((uint32_t)degrees * STEPS_PER_REVOLUTION) / 360);

	rotateSteps(steps, direction);		//지정된 방향으로 회전
}

void rotate_1cm(uint8_t dir)
{
	 //전층 운행
	  HAL_TIM_Base_Start_IT(&htim11);
	  rotateDegrees(150, dir);  //(1250으로 모터 회전 조절)cw:시계/ccw:반시계

}


//void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
//{
//
//	HAL_TIM_Base_Stop_IT(&htim11);
//}

