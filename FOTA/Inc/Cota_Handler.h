/*
 * Cota_Handler.h
 *
 *  Created on: Aug 8, 2024
 *      Author: Riya
 */

#ifndef SRC_COTA_HANDLER_H_
#define SRC_COTA_HANDLER_H_

#include "main.h"
#include "internalFlashStore.h"
#include "gpio_handler.h"
#include "TML.h"

typedef enum{
	COTA_SMS,
	COTA_UART,
	COTA_TCP
}CotaType;

bool getDeviceInfo(char *DataBuffer);
bool getDeviceStatus(char *DataBuffer);
bool GetGps(char *DataBuffer);
bool GetVer(char *DataBuffer);
void SET_FOTA_UPDATE(char *Version);
void setUpdateStaus(bool upStatus);
bool COTA_process(CotaType cotaType, char *cotaBuff, char *cotaResponseBuff);

bool getUpdateStatus();
void setUpdateStaus(bool upStatus);
bool parseCommandSMS(char *SMSBuff, char *opBuffer);
void SET_RESET();
bool GetHistoryStack(char *DataBuffer);
#endif /* SRC_COTA_HANDLER_H_ */
