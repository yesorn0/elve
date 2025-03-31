/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "elevator.h"
#include "DHT11.h"
#include "water.h"
#include "buzzer.h"
#include "i2c_lcd.h"
#include "ultrasonic.h"
#include "fnd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    ALWAY,       // 평상시
    OPENCLOSE,  // 문 열림, 닫힘
    DANGER,      // 위험
} LcdFlag;

LcdFlag lcd_flag = ALWAY;
uint8_t presence;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRIG_PORT GPIOA
#define TRIG_PIN  GPIO_PIN_5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#ifdef __GNUC__
/* With GCC small printf (option LD Linker->Libraries->Small printf
 * set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int  __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int  fputc(int ch, FILE *f)
#endif /* __GNUC__*/

/** @brief Retargets the C library printf function to the USART.
 *  @param None
 *  @retval None
 */
PUTCHAR_PROTOTYPE {
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);
    if (ch == '\n') {
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r", 1, 0xFFFF);
    }
    return ch;
}


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t distance_F;
extern uint8_t distance_B;
volatile uint8_t rxdata1;
volatile uint8_t rxdata2;
volatile uint8_t rxdata3;
uint8_t presence;
uint8_t temp;
uint8_t can_operate_flag = 1;       // 엘리베이터 작동 가능 여부 플래그 (1: 가능, 0: 불가능)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void temp_process(void);
void water_process(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_TIM10_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM11_Init();
  MX_SPI3_Init();
  MX_TIM5_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim11);
  HAL_TIM_Base_Start(&htim5);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);// 서보모터
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//부저
  //rotateDegrees(400, DIR_CCW);
  //rotateDegrees(400, DIR_CCW);
  printf("System Started\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  temp_process();//데이터를 읽어와서 저장을 할거란 말이야/
	  if(can_operate_flag)
	  {
		  elevator_process();
	  }
      HAL_Delay(4000);
//	  rxdata1=Get_3cm_photo_data();
//	  rxdata2=Get_9cm_photo_data();
//	  rxdata3=Get_15cm_photo_data();
//
//	  HAL_Delay(500);
//    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void temp_process(void)
{

	 DHT11_Init();
	 presence=DHT11_Check_Response();
	 if(presence == -1)
	 {
		 printf("No sensor data \r\n");
	 }
	 else if(presence == 1)
	 {
		 DHT11_Process();
	 }
	 delay_DHT11(3000);
}

void water_process(void)
{
	uint8_t water_warning_alarm;
	water_warning_alarm=WATER_Check();
	if(!water_warning_alarm)
	{
	    can_receive_req_B1F=0;//요청 못 받게 함.
	    printf("Cannot receive data for B1F\r\n");
	}
	else if(water_warning_alarm)
	{
		can_receive_req_B1F=1;
		printf("Can  receive data for B1F\r\n");
	}
	HAL_Delay(20000);
}



// lcd_flag에 따라 LCD 출력이 바뀌도록
void LCD_process()
{
   if(lcd_flag == ALWAY)
   {
      char str[15];
      lcd_command(CLEAR_DISPLAY);
      HAL_Delay(100);
      move_cursor(0, 0);
      printf("Humidty: %d, Temperature :%d", RH_byte1, Temp_byte1);
      lcd_string(str);
   }
   else if(lcd_flag == OPENCLOSE)
   {
      lcd_command(CLEAR_DISPLAY);
      HAL_Delay(100);
      move_cursor(0, 0);
      lcd_string("Hello~! Bye~!");
   }
   else if(lcd_flag == DANGER)
   {
      lcd_command(CLEAR_DISPLAY);
      HAL_Delay(100);
      move_cursor(0, 0);
      lcd_string("Emergency!!");
   }
}


void FND_process()
{
   uint8_t fnd_print;
   fnd_print = current_floor - 1; // 현재 층 받아와서 (여기 값 받아와야됨)

   FND_On(fnd_print);
}



void ULTRA_process()
{
   HCSR04_Trigger(GPIOA, GPIO_PIN_10, TIM_IT_CC1);
   HCSR04_Trigger(GPIOA, GPIO_PIN_11, TIM_IT_CC2);

   printf("ch1 : %d cm, ch2 : %d cm\r\n", distance_F, distance_B);

   // LED output 핀 열어서 아래 핀번호 입력해줘야함
   // 지상 사람 감지 (LED)
   if(distance_F < 3)
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
   else
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);

   // 지하 사람 감지
   if(distance_B < 3)
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
   else
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
