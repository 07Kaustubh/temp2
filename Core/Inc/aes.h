/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    aes.h
  * @brief   This file contains all the function prototypes for
  *          the aes.c file
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
#ifndef __AES_H__
#define __AES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "internalFlashStore.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CRYP_HandleTypeDef hcryp;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_AES_Init(void);

/* USER CODE BEGIN Prototypes */
void sKey_update(int key);
void cKey_update(int key);
void Decrypt(char Data[], uint8_t De_data[], int Key, int KeyType);
int encrypt(char Data[], uint8_t En_data[]);
uint8_t calculateChecksum(uint8_t *dataBuffer);
void addPadding(uint8_t *dataBody, size_t string_length);
void hexStringToUint32Array(const char *hexString, uint32_t *uintArray);
void string_to_hex_array(const char hex_string [],uint8_t hex_array[]);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __AES_H__ */

