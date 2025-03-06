/*
 * FOTA.h
 *
 *  Created on: Apr 8, 2024
 *      Author: Manky
 */

#ifndef INC_FOTA_H_
#define INC_FOTA_H_

#include "main.h" // Include necessary header file
#include "stdbool.h"
#include "EC200Utils.h"
#include "common.h"
#include "internalFlashStore.h"
#include "aes.h"
#include "Flash.h"
#include "TML.h"
#include "http.h"
#include "LC86.h"
//#include "internalFlashStore.h"
//#include"EC200Utils.h"
#include "cJSON.h"
//#include "TML.h"
#define header "$MMI"
//#define URL "https://intouchdev.mapmyindia.com/IntouchFOTACOTA/"
#define BS 1024
typedef struct {
	char version[50];
	char configVersion[50];
	char updateVersion[50];
	char updateConfigVersion[50];
	int configUpdateRequired;
	int versionUpdateRequired;
	char errorResponse[100];
	int status;
	int logType;
} FOTA_Status;

typedef enum{
	PACKET_ONE_FAIL,
	PACKET_THREE_FAIL,
	PACKET_SIX_FAIL
}HttpPacketCount;
typedef enum {
    FOTA_FLAG_SKEY = 0,  // Bit 0
    FOTA_FLAG_CKEY,      // Bit 1
    FOTA_FLAG_URL,       // Bit 2
    FOTA_FLAG_POST,      // Bit 3
    FOTA_FLAG_READ,      // Bit 4
    FOTA_FLAG_UPDATE,    // Bit 5
    FOTA_FLAG_ERROR,     // Bit 6
    FOTA_FLAG_RESERVED   // Bit 7
} FotaFlag;
#define EC_TOUT (int*)80  // AT-commands Response Timeout
#define Delimiter '*'
#define KeyCode 1

#define CRC_POLYNOMIAL 0x104C11DB7
#define CRC_INITIAL_VALUE 0xFFFFFFFF
#define CRC_XOR_OUT 0xFFFFFFFF

/****************************************************** CONFIG SECTOR ADDRESS *****************************************************/
//										 Memory range  Sector 179 0x08066000   -    0x08067FFF

#define Config_Data ((uint32_t)0x08166000) //Sector 179
#define S_Key_ADDR ((uint32_t)0x08166010)

// For REFERENCE
// #define S_Key2_ADDR ((uint32_t)0x08066020)
// #define S_Key3_ADDR ((uint32_t)0x08066030)

#define C_Key_ADDR ((uint32_t)0x08166080)

// For REFERENCE
//#define C_Key2_ADDR ((uint32_t)0x08066050)
//#define C_Key3_ADDR ((uint32_t)0x08066060)
//#define C_Key4_ADDR ((uint32_t)0x08066070)
//#define C_Key5_ADDR ((uint32_t)0x08066080)
//#define C_Key6_ADDR ((uint32_t)0x08066090)
//#define C_Key7_ADDR ((uint32_t)0x080660A0)
//#define C_Key8_ADDR ((uint32_t)0x080660B0)

//#define App1_LOC_Addr ((uint32_t)0x081660C0)   // GI
//#define App2_LOC_Addr ((uint32_t)0x081661D0)
//#define App3_LOC_Addr ((uint32_t)0x081661E0)
#define Last_App_ver_Addr ((uint32_t)0x081661F0)
//#define Current_App_LOC ((uint32_t)0x08168000) // Sector 180
#define Imei_Addr ((uint32_t)0x08166120)
//   											Unallocated Range from 0x08167000
/*___________________________________________________END OF MEMORY ALLOCATION ____________________________________________________*/


void Config_Update(char* cmnds, char* Data);
int Response_Check(char Buff[]);
void Message_Type(int Msg_ID);
bool processMessage(int messageType);
void Seprate_String(char Buff[], int Start_Location, int index, char *data[]);
void Extract_Data_from_Buffer(int Start_loc, int End_loc, char Buff[], char *extractedData, int *size);
void string_to_hex_array(const char hex_string [], uint8_t hex_array[] );
int Hex_Check(char Buff[], int hexValue);
void Fetch_Size(uint8_t buffer[], int content_length);
void FOTA_Seprate_String(char Buff[], int Start_Location, int index,char *data[], char Encpt_Data[]);
//bool EC20_Transmit(const char *command,char *resBuff);
//bool wait_for_response(char *resBuff);
uint8_t Calculate_CRC8(uint8_t *data, uint32_t size);
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
//void Ec200_RxEventCallback(uint16_t size);
bool Extract_FOTA_status();
bool setStaticKeys(char *key1,char *key2,char *key3);
bool EC200_HTTP_POST(int type);
bool EC200_File_save(long int,char*);
//bool EC200_Config_Update(const char *command)
bool handleIntouchPost(int fotaMsgType,char *keyValue);
bool reset_fota_temp_buff();
bool fota_seprate_string(char Buff[],int index,char *data[], char Encpt_Data[]);
bool handleKeysRequest();
bool handleFotaDetails();
bool handleFotaData();
bool handleFotaACK(int );
void handleCotaData();
void handleErrorResponse() ;
bool extractKeys(const char *);
bool check_fota_retry();
#endif /* INC_FOTA_H_ */
