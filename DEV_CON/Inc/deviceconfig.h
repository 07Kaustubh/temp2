/*
 * Device_Config.h
 * Created on: Sep 19, 2024
 * Author: Shekhar Verma
 */

#ifndef INC_DEVICECONFIG_H_
#define INC_DEVICECONFIG_H_

#include "common.h"
#include "ctype.h"
#include "stdbool.h"
#include "stdio.h"
#include "Cota_Handler.h"
#include "stdlib.h"
#include "string.h"

#define Comma	0x2C
#define Collon	0X3A

#define MAX_LENGTH 100
/*~~~~~~~~~~~~~~~~~~~~FRAME Type~~~~~~~~~~~~~~~~~~~~~*/
#define DEV_GET 0xDA
#define DEV_SET 0xDB
#define DEV_TEST 0xDC
/*~~~~~~~~~~~~~~~~~~~~SEPERATOR~~~~~~~~~~~~~~~~~~~~~~*/
#define DEV_ID_SEPERATOR 0xD7
#define DEV_DATA_SEPERATOR 0xD5
#define DEV_FOOTER	0xD1

typedef struct {
	uint8_t Header[5];	//$mmi
	uint32_t Key;	//1234 seed key
	uint8_t Frametype;	//set or get
	uint8_t DataPacket[1024];	// here data is there
	uint8_t checksum;		//checksum
}DEV_Config_Status;

void generate_get_string(int index,char *buffer,char code1,char code2);
bool parseData(uint8_t *dataBuffer,uint8_t *dataToSend);
uint8_t escape_character(uint8_t escapeValue,uint8_t value);
//bool parseData(uint8_t *dataBuffer,uint8_t *dataToSend);
int splitHexString(uint8_t *string, uint8_t delimiter, uint8_t opbuffer[][100], size_t bufsize);
#endif /* INC_DEVICECONFIG_H_ */
