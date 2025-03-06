


#ifndef INC_EC200UTILS_H_
#define INC_EC200UTILS_H_
#include "main.h"
#include "stm32h5xx_hal.h"
#include "common.h"
#include "memorymap.h"
#include "TML.h"
//#include <ctype.h>
//#include "internalFlashStore.h"
#define RESBUFF_SIZE 2048
#define EC200MINRESONSETIME 1000
#define EC200MAXRESONSETIME 10000
#define IMEI_LENGTH 15
/******************************************* END Defination ********************************************/

///*******************************************PIN SETUP**************************************************/
//moved to gpio.h
//#define GNSS_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET) // Set PD2 high GNSS ON/OFF
//#define GNSS_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
//
//#define GPIO_LTE_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET)
//#define GPIO_LTE_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET) // Set PC9 high LTE ON/OFF SET=OFF,RESET=ON
//
//#define GPIO_3V3_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) // Set PB3 high 3.3V ON/OFF
//#define GPIO_3V3_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
//
//#define GPS_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
//#define GPS_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)
//
//#define GSM_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
//#define GSM_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
//
//#define PWR_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
//#define PWR_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
/*******************************************END SETUP**************************************************/

void enableIdle();
bool EC200_Init(void);
void E200_RXCpltCallback(void);
void E200_RxHAlfCpltCallback(void);
void Ec200_RxEventCallback(uint16_t size);
//bool EC200_Transmit(const char *,char *,int);
bool setupEc200(UART_HandleTypeDef *uart);
void setupEc200MQTT(UART_HandleTypeDef *uart);
bool wait_for_response(char *resBuff,int timeout);
void switchIdle(bool idleFlag);
bool get_MCC_MNC_CellID() ;
bool wait_for_response_TCP(char *resBuff,char *compareBuff,int timeout);
//bool EC200_Transmit_TCP(const char *command,char *resBuff,char *compareBuff,int timeout);
bool getIMEI(char *opbuffer);
int getSignal();
bool set_MCC_MNC_CellID() ;
bool getOperatorName(char *opbuffer);
void set_ec200_response_check(const uint8_t *);
bool compare_ec200_response(char *buffer);
bool isNumeric(const char *str, int length);
bool getICCID(char *opbuffer);
bool get_gnss_time_date();
bool EC200_Transmit(const char *command,char *resBuff,int timeout);
#endif
