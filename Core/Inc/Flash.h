/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash.h
  * @brief   This file contains all the function prototypes for
  *          the flash.c file
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
#ifndef __flash_H__
#define __flash_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x081E0000) /* Base @ of SECTOR 1, 128 Kbytes */

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_7   /* Start @ of user Flash area */


#define  Start_Address			(ADDR_FLASH_SECTOR_1)


extern uint32_t FirstSector,
                NbOfSectors,
                BankNumber,
                Address,
                SectorError;
/* USER CODE END Private defines */

void MX_FLASH_Init(void);

/* USER CODE BEGIN Prototypes */

uint32_t GetSector(uint32_t Address);
uint32_t GetBank(uint32_t Address);
uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data);
void Program_Flash(void);
void HAL_FLASH_READ(char *Buff, uint32_t Addr, int Buff_Lnt);
bool HAL_FLASH_Write_Buff(uint32_t FlashAddress, uint8_t *Data, uint32_t DataSize);
void HAL_FLASH_ERASE(uint32_t FlashEraseAddress);
void Write_Config(void);
void Get_Config(void);
void Get_Factory(void);
void Flash_BKP_Parameters(void);
char* GET_DID_Param(uint8_t Parameter_No);
void Erase_sectors(uint32_t FlashAddress);
void GET_DID_Parameter(uint8_t Parameter_No, char* buffer);
uint64_t reverseByteOrder(uint64_t input);
void FLASH_Read_Buff(uint32_t Read_Address, char Read_Buff[],
		uint32_t ReadDataSize);
extern FLASH_EraseInitTypeDef EraseInitStruct;  // Erase structure
bool CopyFlashSectors(uint32_t sourceAddr, uint32_t destAddr, uint32_t size);
void writeF_CP(uint64_t CData, uint64_t CData2, uint64_t DData3);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __flash_H__ */

