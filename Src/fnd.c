#include "fnd.h"

uint32_t seg[10] = {
		0b00111111, // 0
		0b00000110, // 1
		0b01011011, // 2
		0b01001111, // 3
		0b01100110, // 4
		0b01101101, // 5
		0b01111101, // 6
		0b00000111, // 7
		0b01111111, // 8
		0b01101111  // 9
};


void FND_On(uint8_t num)
{
	GPIOC->ODR = seg[num]; // LED 켜기
}

void FND_Off()
{
	GPIOC->ODR = 0b00000000;
}

