/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

#include "stm32h5xx_ll_rcc.h"
#include "stm32h5xx_ll_spi.h"
#include "stm32h5xx_ll_system.h"
#include "stm32h5xx_ll_gpio.h"
#include "stm32h5xx_ll_exti.h"
#include "stm32h5xx_ll_bus.h"
#include "stm32h5xx_ll_cortex.h"
#include "stm32h5xx_ll_utils.h"
#include "stm32h5xx_ll_pwr.h"
#include "stm32h5xx_ll_dma.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include "time.h"
#include "gpio.h"
//#include "crc.h"
#include "usart.h"
#include "common.h"

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
#define Internal_Battery_Pin LL_GPIO_PIN_0
#define Internal_Battery_GPIO_Port GPIOC
#define EBV_SENSE_Pin LL_GPIO_PIN_1
#define EBV_SENSE_GPIO_Port GPIOA
#define SPI1_CS_Pin LL_GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define TTL_TX_Pin LL_GPIO_PIN_6
#define TTL_TX_GPIO_Port GPIOC
#define TTL_RX_Pin LL_GPIO_PIN_7
#define TTL_RX_GPIO_Port GPIOC
#define RingIndicator_Pin LL_GPIO_PIN_8
#define RingIndicator_GPIO_Port GPIOA
#define RingIndicator_EXTI_IRQn EXTI8_IRQn
#define SPI3_CS_Pin LL_GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define LC86_ON_OFF_Pin LL_GPIO_PIN_2
#define LC86_ON_OFF_GPIO_Port GPIOD
#define ON_OFF_3_3_Pin LL_GPIO_PIN_3
#define ON_OFF_3_3_GPIO_Port GPIOB
#define LED_GPS_Pin LL_GPIO_PIN_4
#define LED_GPS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
