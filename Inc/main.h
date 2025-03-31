/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUTTON_2F_DOWN_Pin GPIO_PIN_0
#define BUTTON_2F_DOWN_GPIO_Port GPIOB
#define BUTTON_2F_DOWN_EXTI_IRQn EXTI0_IRQn
#define BUTTON_1F_UP_Pin GPIO_PIN_1
#define BUTTON_1F_UP_GPIO_Port GPIOB
#define BUTTON_1F_UP_EXTI_IRQn EXTI1_IRQn
#define BUTTON_1F_DOWN_Pin GPIO_PIN_2
#define BUTTON_1F_DOWN_GPIO_Port GPIOB
#define BUTTON_1F_DOWN_EXTI_IRQn EXTI2_IRQn
#define PHOTO_3CM_Pin GPIO_PIN_10
#define PHOTO_3CM_GPIO_Port GPIOB
#define PHOTO_9CM_Pin GPIO_PIN_13
#define PHOTO_9CM_GPIO_Port GPIOB
#define PHOTO_15CM_Pin GPIO_PIN_14
#define PHOTO_15CM_GPIO_Port GPIOB
#define BUTTON_B1F_UP_Pin GPIO_PIN_3
#define BUTTON_B1F_UP_GPIO_Port GPIOB
#define BUTTON_B1F_UP_EXTI_IRQn EXTI3_IRQn
#define BUTTON_2F_SELECT_Pin GPIO_PIN_5
#define BUTTON_2F_SELECT_GPIO_Port GPIOB
#define BUTTON_2F_SELECT_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON_1F_SELECT_Pin GPIO_PIN_6
#define BUTTON_1F_SELECT_GPIO_Port GPIOB
#define BUTTON_1F_SELECT_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON_B1F_SELECT_Pin GPIO_PIN_7
#define BUTTON_B1F_SELECT_GPIO_Port GPIOB
#define BUTTON_B1F_SELECT_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
