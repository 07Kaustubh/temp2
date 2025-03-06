/*
 * LC86.h
 *
 *  Created on: Apr 9, 2024
 *      Author: Nk
 */

#ifndef INC_LC86_H_
#define INC_LC86_H_
#include "main.h"
#include"stm32h5xx_hal.h"
#include"common.h"
#include"stdbool.h"
#include"stdlib.h"
#include "string.h"
#include"stdio.h"
#include "ledblink.h"
/* ********************************************************* Define ********************************************************/
//#define GNSS_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET) // Set PD2 high GNSS ON/OFF
//#define GNSS_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
//
//#define POWER_3V3_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) // Set PB3 high 3.3V ON/OFF
//#define POWER_3V3_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
//
//#define GPS_LIGHT_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
//#define GPS_LIGHT_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

#define L86_BUFFER_SIZE 1000
/* ********************************************************* Function Definition ********************************************************/
void L86_setup(UART_HandleTypeDef *uart);
void L86_uart_Init(void);
void CollectL86Data(void);
void reset_l86_time();
typedef struct GPSData{
		double latitude;//GNRMS
		char lat_dir; //GNRMS
		double longitude;//GNRMS
		char lon_dir;//GNRMS
		float speed;//GNRMS
		int heading;//GNRMS
		uint8_t sat_num;//GNGGA
		int alt;//GNGGA
		float PDOP;//GNGGA
		float HDOP;//GNGGA
		bool gps_fix; //GNGGA
		/*********************************LinkList***********************************************/
		struct GPSData *nextData;
}GPSData;
//typedef void (*L86Callback)(GPSData*);
bool init_GPSData(GPSData *GPSDataTmp);
__weak void GPSDataCallback(GPSData*);
void L86_RxCpltCallback();
void L86_RxHalfCpltCallback();
void gnss_data(char*l89_buffer);
void gnss_data_on();
void gnss_data_off();
//void gnss_data_on()
#endif /* INC_LC86_H_ */
