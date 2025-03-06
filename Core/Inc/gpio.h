/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define V2
/* USER CODE END Private defines */

void MX_GPIO_Init(void);
//void IGN_GPIO_Init(void);
/* USER CODE BEGIN Prototypes */
/*******************************************PIN SETUP**************************************************/
//#ifdef V2
#define CAN_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
#define CAN_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)

#define GPIO_LTE_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET)
#define GPIO_LTE_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET)

#define CAN_STBY_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define CAN_STBY_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)

#define D1_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define D1_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)

#define D2_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)
#define D2_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)

#define CHG_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
#define CHG_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)

#define GPIO_3V3_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET) // Set PD2 high GNSS ON/OFF
#define GPIO_3V3_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
//#else
//#define GPIO_LTE_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET)
//#define GPIO_LTE_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET) // Set PC9 high LTE ON/OFF SET=OFF,RESET=ON
//#define GPIO_3V3_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) // Set PB3 high 3.3V ON/OFF
//#define GPIO_3V3_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
//#define GPIO_LTE_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET)
//#define GPIO_LTE_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET) // Set PC9 high LTE ON/OFF SET=OFF,RESET=ON
//#endif

//#define GNSS_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET) // Set PD2 high GNSS ON/OFF
//#define GNSS_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)

//#define GPIO_3V3_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) // Set PB3 high 3.3V ON/OFF
//#define GPIO_3V3_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)

#define GPS_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define GPS_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

#define GSM_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define GSM_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#define PWR_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define PWR_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

