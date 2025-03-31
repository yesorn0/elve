#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "main.h"
#include "tim.h"
#include "delay.h"

void HCSR04_Trigger(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t TIM_IT_CCx);

#endif /* INC_ULTRASONIC_H_ */
