/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    memorymap.h
 * @brief   This file contains all the function prototypes for
 *          the memorymap.c file
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
#ifndef __MEMORYMAP_H__
#define __MEMORYMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#define SMS_COTA_SEC	((uint32_t)0x0816A000)
#define Current_App_LOC ((uint32_t)0x08168000) // Sector 180
#define SMALL_BUFFER_SIZE 16
#define LARGE_BUFFER_SIZE 33


typedef enum  {
	//length max 16 flash allocated 32
	Config_MIP,
	Config_MPORT1,
	Config_MUSER,
	Config_MPASS,
	Config_CERT,
	Config_KEY,
	Config_CA,
	Config_IP2,
	Config_IPPORT2,

	Config_ION,
	Config_NSAM,
	Config_HA,
	Config_HB,
	Config_HC,
	Config_ACCOFFSET,
	Config_USER,
	Config_PSWD,

	Config_LAT,
	Config_LON,
	Config_PHNUM,
	Config_FOTATime,
	//write all parameteres length required 32 to 63 allocated 64

	Config_DAPN,
	Config_VIN,
	Config_CKEY1,
	Config_CKEY2,
	Config_CKEY3,
	Config_CKEY4,
	Config_CKEY5,
	Config_CKEY6,
	Config_CKEY7,
	Config_CKEY8,
	Config_SKEY1,
	Config_SKEY2,
	Config_SKEY3,

	Config_SMS_number,
	Config_PageCount,
	Config_BlockCount,
	Config_SleepFlag,
	Config_LastFotaTime,
	Config_PowerFlag,
	Config_WAKEUP_TIMER,
	/////////////////new added ////////////
	Config_DOUT1,
	Config_DOUT2,
	Config_IMZ,
	Config_CanF0,
	Config_CanF1,
	Config_CanF2,
	Config_CanF3,
	Config_CanF4,
	Config_CanF5,
	Config_CanF6,
	Config_CanF7,
	Config_CanNbr,
	/********this are only get function*******/
	Config_CAN_TX1,
	Config_CAN_TX2,
	Config_CAN_TX3,
	Config_DIN1,
	Config_DIN2,
	Config_IMEI,
	Config_ICCID,
	Config_FW_VER,
	Config_HW_VER,
	Config_CNF_VER,
	Config_ModelName,
	/***************************/
	CONFIG_COUNT // to know total configs
}ConfigParametersID ;

typedef struct {
	ConfigParametersID key;
	char* keyString;
	uint32_t address;
	char* defaultValue;
	uint8_t size;
} ConfigItem;

typedef struct{
	ConfigItem valueOfConfig[CONFIG_COUNT];
	int totalData;
} ConfigTable;

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __MEMORYMAP_H__ */

