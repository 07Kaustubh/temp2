/*
 * CAN.h
 *
 *  Created on: Apr 16, 2024
 *      Author: 91944
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_
#include"stm32h5xx_hal.h"
#include"map.h"
#include "gpio.h"
#define NUM_PID 35
void getCANData(Map *canDataCopy);
void processCAN1(uint32_t id,uint8_t *data,uint8_t length);
void initCan(void);
void TX_DataCAN(const uint8_t *TxData);
#endif /* INC_CAN_H_ */
