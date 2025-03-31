/*
 * DHT11.h
 *
 *  Created on: Mar 24, 2025
 *      Author: USER
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include "tim.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_4

extern uint8_t RH_byte1, RH_byte2, Temp_byte1, Temp_byte2;
extern uint16_t CHECKSUM, RH, TEMP;

extern float Temperature;
extern float Humidity;
extern char msg[50];

void delay_DHT11(uint16_t time);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT11_Init(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read(void);
void DHT11_Process(void);
void Transmit(char *message);

#endif /* INC_DHT11_H_ */
