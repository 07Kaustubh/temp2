/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_HandleTypeDef *rtc;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

int WAkeuptimer = 0;
extern bool sleepStatus;
extern RTC_HandleTypeDef hrtc;
extern uint8_t StrtFotaTmbuf[2], getFotaTmbuf[2];
extern bool sendPDDataFlag, PowerOnFlag, powerOnCount;

long int currentEpochTime, lastFotaTime;
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
    __HAL_RCC_RTCAPB_CLK_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
bool getSleepStatus() {
	return sleepStatus;
}
void setSleppStatus(bool sleep) {
	sleepStatus = sleep;
}
void initRTC(RTC_HandleTypeDef *hrtc) {
	rtc = hrtc;
}
bool setTime(uint8_t hours, uint8_t minute, uint8_t second) {
	clearBuffer((uint8_t*) &sTime, sizeof(sTime));
	sTime.Hours = hours;
	sTime.Minutes = minute;
	sTime.Seconds = second;


	if (HAL_RTC_SetTime(rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		// Handle error
		return false;
	}
	return true;
}

bool setDate(uint8_t date, uint8_t month, uint8_t year) {
	clearBuffer((uint8_t*) &sDate, sizeof(sDate));
	sDate.Date = date;
	sDate.Month = month;
	sDate.Year = year;

	if (HAL_RTC_SetDate(rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		return false;
	}
	return true;
}
time_t convert_to_epoch(RTC_TimeTypeDef *time, RTC_DateTypeDef *date) {
    struct tm t;
    t.tm_year = date->Year + 100; // Year since 1900, assuming date->Year is in 0-99 format
    t.tm_mon = date->Month - 1;   // Month, where 0 = Jan
    t.tm_mday = date->Date;
    t.tm_hour = time->Hours;
    t.tm_min = time->Minutes;
    t.tm_sec = time->Seconds;
    t.tm_isdst = -1;              // No daylight saving time

    return mktime(&t);
}
bool getDateTime(char opDate[], char opTime[]) {
	clearBuffer((uint8_t*) &sTime, sizeof(sTime));
	clearBuffer((uint8_t*) &sDate, sizeof(sDate));
	if (HAL_RTC_GetTime(rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		return false;
	}
	if (HAL_RTC_GetDate(rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		return false;
	}
	currentEpochTime=convert_to_epoch(&sTime, &sDate);
	sprintf(opDate, "%02d%02d20%02d", sDate.Date, sDate.Month, sDate.Year);
	sprintf(opTime, "%02d%02d%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
	return true;
}

double get_Fotatime_difference(time_t start_epoch, time_t end_epoch) {

long int epochTimeDiff=end_epoch-start_epoch;
    return epochTimeDiff;
}

void Configure_RTC_WakeUp_Timer() {

	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, WAkeuptimer,
	RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0) != HAL_OK) {
		Error_Handler();
	}

}
/* USER CODE END 1 */
