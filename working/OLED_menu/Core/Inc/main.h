/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define button_left_Pin GPIO_PIN_1
#define button_left_GPIO_Port GPIOC
#define button_left_EXTI_IRQn EXTI1_IRQn
#define button_up_Pin GPIO_PIN_3
#define button_up_GPIO_Port GPIOC
#define button_up_EXTI_IRQn EXTI3_IRQn
#define button_select_Pin GPIO_PIN_0
#define button_select_GPIO_Port GPIOA
#define button_select_EXTI_IRQn EXTI0_IRQn
#define button_down_Pin GPIO_PIN_4
#define button_down_GPIO_Port GPIOA
#define button_down_EXTI_IRQn EXTI4_IRQn
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define button_right_Pin GPIO_PIN_6
#define button_right_GPIO_Port GPIOA
#define button_right_EXTI_IRQn EXTI9_5_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
