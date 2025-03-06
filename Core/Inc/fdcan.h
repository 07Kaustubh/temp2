/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
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
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "stdlib.h"
#include "internalFlashStore.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

extern FDCAN_HandleTypeDef hfdcan2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);
void MX_FDCAN2_Init(void);
//int splitHexString_01(uint8_t *string, size_t length, uint8_t delimiter, uint8_t opbuffer[][100], size_t bufsize);
/* USER CODE BEGIN Prototypes */
typedef struct {
	uint32_t ConfigIdType[8];
	uint8_t configFilterType[8];
	uint32_t ConfigFilter1[8];
	uint32_t ConfigFilter2[8];
} RxCanConfiguration;
//typedef struct {
//	uint32_t ConfigIdType;
//	uint32_t ConfigFilter;
//	uint8_t ConfigData;
//} TxCanConfiguration;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

