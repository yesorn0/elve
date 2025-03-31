/*
 * door.c
 *
 *  Created on: Mar 27, 2025
 *      Author: USER
 */
#include "door.h"

void Servo_Rotate(uint16_t target_ccr, uint16_t speed_delay_ms)
{
  uint16_t current_ccr = TIM5->CCR2; // 현재 CCR1 값 가져오기

  // 현재 값에서 목표 값까지 점진적으로 이동
  if (current_ccr < target_ccr)
  {
    for (uint16_t i = current_ccr; i <= target_ccr; i++)
    {
      current_ccr = i;           // CCR1 값 설정
      HAL_Delay(speed_delay_ms); // 속도 조절을 위한 지연
    }
  }
  else
  {
    for (uint16_t i = current_ccr; i >= target_ccr; i--)
    {
      current_ccr = i;           // CCR1 값 설정
      HAL_Delay(speed_delay_ms); // 속도 조절을 위한 지연
    }
  }
}
