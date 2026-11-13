/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, DIG3_3_Pin|DIG3_4_Pin|DIG3_5_Pin|DIG3_6_Pin
                          |DIG3_7_Pin|DIG3_8_Pin|DIG4_1_Pin|DIG4_2_Pin
                          |DIG4_3_Pin|DIG4_4_Pin|DIG4_5_Pin|DIG4_6_Pin
                          |DIG4_7_Pin|DIG4_8_Pin|DIG3_1_Pin|DIG3_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DIG5_3_Pin|DIG5_4_Pin|DIG5_5_Pin|DIG5_6_Pin
                          |DIG5_7_Pin|DIG5_8_Pin|DIG5_1_Pin|DIG5_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, DIG2_1_Pin|DIG2_2_Pin|DIG2_3_Pin|DIG2_4_Pin
                          |DIG2_5_Pin|DIG2_6_Pin|DIG2_7_Pin|DIG2_8_Pin
                          |DIG1_1_Pin|DIG1_2_Pin|DIG1_3_Pin|DIG1_4_Pin
                          |DIG1_5_Pin|DIG1_6_Pin|DIG1_7_Pin|DIG1_8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DIG3_3_Pin DIG3_4_Pin DIG3_5_Pin DIG3_6_Pin
                           DIG3_7_Pin DIG3_8_Pin DIG4_1_Pin DIG4_2_Pin
                           DIG4_3_Pin DIG4_4_Pin DIG4_5_Pin DIG4_6_Pin
                           DIG4_7_Pin DIG4_8_Pin DIG3_1_Pin DIG3_2_Pin */
  GPIO_InitStruct.Pin = DIG3_3_Pin|DIG3_4_Pin|DIG3_5_Pin|DIG3_6_Pin
                          |DIG3_7_Pin|DIG3_8_Pin|DIG4_1_Pin|DIG4_2_Pin
                          |DIG4_3_Pin|DIG4_4_Pin|DIG4_5_Pin|DIG4_6_Pin
                          |DIG4_7_Pin|DIG4_8_Pin|DIG3_1_Pin|DIG3_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : TRIG_SIG1_Pin TRIG_SIG2_Pin */
  GPIO_InitStruct.Pin = TRIG_SIG1_Pin|TRIG_SIG2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Controll_Mode_Pin */
  GPIO_InitStruct.Pin = Controll_Mode_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Controll_Mode_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIG5_3_Pin DIG5_4_Pin DIG5_5_Pin DIG5_6_Pin
                           DIG5_7_Pin DIG5_8_Pin DIG5_1_Pin DIG5_2_Pin */
  GPIO_InitStruct.Pin = DIG5_3_Pin|DIG5_4_Pin|DIG5_5_Pin|DIG5_6_Pin
                          |DIG5_7_Pin|DIG5_8_Pin|DIG5_1_Pin|DIG5_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DIG2_1_Pin DIG2_2_Pin DIG2_3_Pin DIG2_4_Pin
                           DIG2_5_Pin DIG2_6_Pin DIG2_7_Pin DIG2_8_Pin
                           DIG1_1_Pin DIG1_2_Pin DIG1_3_Pin DIG1_4_Pin
                           DIG1_5_Pin DIG1_6_Pin DIG1_7_Pin DIG1_8_Pin */
  GPIO_InitStruct.Pin = DIG2_1_Pin|DIG2_2_Pin|DIG2_3_Pin|DIG2_4_Pin
                          |DIG2_5_Pin|DIG2_6_Pin|DIG2_7_Pin|DIG2_8_Pin
                          |DIG1_1_Pin|DIG1_2_Pin|DIG1_3_Pin|DIG1_4_Pin
                          |DIG1_5_Pin|DIG1_6_Pin|DIG1_7_Pin|DIG1_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
