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
#define DIG3_3_Pin GPIO_PIN_2
#define DIG3_3_GPIO_Port GPIOE
#define DIG3_4_Pin GPIO_PIN_3
#define DIG3_4_GPIO_Port GPIOE
#define DIG3_5_Pin GPIO_PIN_4
#define DIG3_5_GPIO_Port GPIOE
#define DIG3_6_Pin GPIO_PIN_5
#define DIG3_6_GPIO_Port GPIOE
#define DIG3_7_Pin GPIO_PIN_6
#define DIG3_7_GPIO_Port GPIOE
#define TRIG_SIG1_Pin GPIO_PIN_6
#define TRIG_SIG1_GPIO_Port GPIOA
#define TRIG_SIG2_Pin GPIO_PIN_7
#define TRIG_SIG2_GPIO_Port GPIOA
#define Controll_Mode_Pin GPIO_PIN_4
#define Controll_Mode_GPIO_Port GPIOC
#define DIG3_8_Pin GPIO_PIN_7
#define DIG3_8_GPIO_Port GPIOE
#define DIG4_1_Pin GPIO_PIN_8
#define DIG4_1_GPIO_Port GPIOE
#define DIG4_2_Pin GPIO_PIN_9
#define DIG4_2_GPIO_Port GPIOE
#define DIG4_3_Pin GPIO_PIN_10
#define DIG4_3_GPIO_Port GPIOE
#define DIG4_4_Pin GPIO_PIN_11
#define DIG4_4_GPIO_Port GPIOE
#define DIG4_5_Pin GPIO_PIN_12
#define DIG4_5_GPIO_Port GPIOE
#define DIG4_6_Pin GPIO_PIN_13
#define DIG4_6_GPIO_Port GPIOE
#define DIG4_7_Pin GPIO_PIN_14
#define DIG4_7_GPIO_Port GPIOE
#define DIG4_8_Pin GPIO_PIN_15
#define DIG4_8_GPIO_Port GPIOE
#define DIG5_3_Pin GPIO_PIN_10
#define DIG5_3_GPIO_Port GPIOB
#define DIG5_4_Pin GPIO_PIN_11
#define DIG5_4_GPIO_Port GPIOB
#define DIG5_5_Pin GPIO_PIN_12
#define DIG5_5_GPIO_Port GPIOB
#define DIG5_6_Pin GPIO_PIN_13
#define DIG5_6_GPIO_Port GPIOB
#define DIG5_7_Pin GPIO_PIN_14
#define DIG5_7_GPIO_Port GPIOB
#define DIG5_8_Pin GPIO_PIN_15
#define DIG5_8_GPIO_Port GPIOB
#define DIG2_1_Pin GPIO_PIN_8
#define DIG2_1_GPIO_Port GPIOD
#define DIG2_2_Pin GPIO_PIN_9
#define DIG2_2_GPIO_Port GPIOD
#define DIG2_3_Pin GPIO_PIN_10
#define DIG2_3_GPIO_Port GPIOD
#define DIG2_4_Pin GPIO_PIN_11
#define DIG2_4_GPIO_Port GPIOD
#define DIG2_5_Pin GPIO_PIN_12
#define DIG2_5_GPIO_Port GPIOD
#define DIG2_6_Pin GPIO_PIN_13
#define DIG2_6_GPIO_Port GPIOD
#define DIG2_7_Pin GPIO_PIN_14
#define DIG2_7_GPIO_Port GPIOD
#define DIG2_8_Pin GPIO_PIN_15
#define DIG2_8_GPIO_Port GPIOD
#define DIG1_1_Pin GPIO_PIN_0
#define DIG1_1_GPIO_Port GPIOD
#define DIG1_2_Pin GPIO_PIN_1
#define DIG1_2_GPIO_Port GPIOD
#define DIG1_3_Pin GPIO_PIN_2
#define DIG1_3_GPIO_Port GPIOD
#define DIG1_4_Pin GPIO_PIN_3
#define DIG1_4_GPIO_Port GPIOD
#define DIG1_5_Pin GPIO_PIN_4
#define DIG1_5_GPIO_Port GPIOD
#define DIG1_6_Pin GPIO_PIN_5
#define DIG1_6_GPIO_Port GPIOD
#define DIG1_7_Pin GPIO_PIN_6
#define DIG1_7_GPIO_Port GPIOD
#define DIG1_8_Pin GPIO_PIN_7
#define DIG1_8_GPIO_Port GPIOD
#define DIG5_1_Pin GPIO_PIN_8
#define DIG5_1_GPIO_Port GPIOB
#define DIG5_2_Pin GPIO_PIN_9
#define DIG5_2_GPIO_Port GPIOB
#define DIG3_1_Pin GPIO_PIN_0
#define DIG3_1_GPIO_Port GPIOE
#define DIG3_2_Pin GPIO_PIN_1
#define DIG3_2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
