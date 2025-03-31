/*
 * DHT11.c
 *
 *  Created on: Mar 24, 2025
 *      Author: USER
 */

#include "DHT11.h"
uint8_t RH_byte1, RH_byte2, Temp_byte1, Temp_byte2;
uint16_t CHECKSUM, RH, TEMP;

float Temperature;
float Humidity;
char msg[50];
void delay_DHT11 (uint16_t time)
{
   __HAL_TIM_SET_COUNTER(&htim2,0);
   while((__HAL_TIM_GET_COUNTER(&htim2))<time);
}
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
   GPIO_InitTypeDef GPIO_InitStruct={0};
   GPIO_InitStruct.Pin=GPIO_Pin;
   GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
   GPIO_InitTypeDef GPIO_InitStruct={0};
   GPIO_InitStruct.Pin=GPIO_Pin;
   GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
   GPIO_InitStruct.Speed=GPIO_NOPULL;
   HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Init(void)
{
   Set_Pin_Output(DHT11_PORT, DHT11_PIN);
   HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);//set pin low
   delay_DHT11(18000); //wait for 18ms
   HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);//set pin high
   delay_DHT11(20);//wait for 20us
   Set_Pin_Input(DHT11_PORT, DHT11_PIN);//set pin input
}

uint8_t DHT11_Check_Response(void)
{
   uint8_t response = 0;
   delay_DHT11(40);
   if(!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
   {
	   delay_DHT11(80);
      if((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
      {
         response = 1;
      }
      else
      {
         response=-1;
      }
   }
   __HAL_TIM_SET_COUNTER(&htim2, 0);
   while((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)&& (__HAL_TIM_GET_COUNTER(&htim2) < 100)));
   return response;
}

uint8_t DHT11_Read(void)
{
   uint8_t i,num;
   for(i=0;i<8;i++)
   {
      while(!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));// wait for the pin to go high
      delay_DHT11(40);
      if(!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))//if pin is low
      {
         num = num & ~(1<<(7-i));
      }
      else//if pin is high
      {
         num |= (1<< (7-i) );
      }

      while((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)&& (__HAL_TIM_GET_COUNTER(&htim2) < 100)));
   }
   return num;
}

void DHT11_Process(void)
{
   RH_byte1=DHT11_Read();
   RH_byte2=DHT11_Read();
   Temp_byte1=DHT11_Read();
   Temp_byte2=DHT11_Read();
   CHECKSUM=DHT11_Read();

   uint8_t humidity_integer =  RH_byte1 ;
   uint8_t humidity_decimal =  RH_byte2 / 10 ;

   uint8_t temperature_integer =  Temp_byte1 ;
   uint8_t temperature_decimal =  Temp_byte2 / 10 ;

   if(CHECKSUM ==((RH_byte1+RH_byte2 +Temp_byte1+Temp_byte2)& 0xff)){
      snprintf(msg,sizeof(msg),"RH = %d.%d %%\r\n",humidity_integer,humidity_decimal);
      Transmit(msg);
      snprintf(msg,sizeof(msg),"temp = %d.%d C\r\n",temperature_integer,temperature_decimal);
      Transmit(msg);
   }
   else
   {
      printf("Checksum Errors ! Trying Again ...\r\n");
   }

}

void Transmit(char *message)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
	//printf(message);
}
