/*
 * TML.h
 *
 *  Created on: Apr 9, 2024
 *      Author: 91944
 */

#ifndef INC_TML_H_
#define INC_TML_H_
#include"stdbool.h"
//#include "stdio.h"
#include"LC86.h"
#include"CAN.h"
#include"FLASH_MEM.h"
#include"fifo.h"
#include"gpio_handler.h"
#include"ledblink.h"
#include "stack.h"
#include "Accelometer.h"
#include "TML_FOTA.h"
#include "Flash.h"
#include "tcp.h"
#include "Cota_Handler.h"
#include "FOTA.h"
#include "sms.h"
#include "gpio.h"
#include "BLE.h"
#include "EC200Utils.h"
#include "error_logger.h"
#include "common.h"
void init();
void loop();
void task2();
void setTickTime();
int getTickTime();
void setIMEI(char *imei);
void gsmParameterUpdate();
void fotaUpdateCallback(char*);
char * getUpdateVersion();
void clearBlockflash();
bool getPrintUartCmdStatus();
void setPrintUartCmdStatus(bool tmpStatus);
void generateData(char *packetType) ;
void Check_USMS();
void clearHistory();
void check_immobilization();
void Sleep_routine();

typedef enum{
	//success
	Request_Received=10,
	Downloading =11,
	Downloaded =12,
	Retry_Downloading=13,
//	error code
	FOTA_URL_Not_Available=20,
	Fota_File_Corrupted=21,
	Permanent_failed =22,
	Memory_allocation_failed=23
}FOTA_ACK_STATUS;
void onPrintUartCmd(char *cmd);
void handleCotaSMS();
void Charging_Switch(void);
void saveTcpHistoryPacket();
void saveTcuInfo();
void Handle_LEDS();
#endif /* INC_TML_H_ */
