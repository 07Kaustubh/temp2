/*
 * sms.h
 *
 *  Created on: May 2, 2024
 *      Author: saura
 */

#ifndef INC_SMS_H_
#define INC_SMS_H_

#include "stm32h5xx_hal.h"
#include "main.h"
#include "icache.h"
//#include "TML.h"





void clear_sms_resp_buff();
bool SMS_Init();
bool SMS_Read(int index, uint8_t* message_buffer);
void setMessageStatus(bool msgStatus);
bool getMessageStatus();
bool SMS_Delete(int index);
void onMessage();
bool SMS_Send(char *number,char *message);
bool SMS_FSend(char *number, char *message);
void check_sms_response();
#endif /* INC_SMS_H_ */
