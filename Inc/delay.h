#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "main.h"
#include "tim.h"

//uint8_t delay_us(uint32_t *prev, uint32_t interval);
void delay_us(uint16_t us);
uint8_t delay_ms(uint32_t *prev, uint32_t interval);

#endif /* INC_DELAY_H_ */
