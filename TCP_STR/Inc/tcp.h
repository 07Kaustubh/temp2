/*
 * tcp.h
 *
 *  Created on: Apr 23, 2024
 *      Author: saura
 */

#ifndef TCP_H_
#define TCP_H_

#include "main.h"
#include "icache.h"

#include "internalFlashStore.h"
#include "EC200Utils.h"
#include "sms.h"
#include "TML.h"
#include "string.h"


extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

typedef enum {
    TCP_FLAG_OPEN = 0,  // Bit 0
    TCP_FLAG_SEND,      // Bit 1
    TCP_FLAG_CLOSE,       // Bit 2
    TCP_FLAG_SERVERCLOSE,      // Bit 3
    TCP_FLAG_ERROR,      // Bit 4
    TCP_FLAG_RESERVED1,    // Bit 5
	TCP_FLAG_RESERVED2,     // Bit 6
    TCP_FLAG_RESERVED3   // Bit 7
} TCPFlag;

void TCP_init();
bool TCP_Open();
bool TCP_config();

bool TCP_Send(const char *data);
bool tcp_close(uint8_t);
bool TCP_DEACT(uint8_t contextID) ;
//bool tcp_receive(char*res_buff);
bool TCP_Config_Context2();
bool resetUEFcuntionSet();
void check_tcp_response();
bool TCP_Send_Receive(const char *data);
bool getTcpConfigStatus();
bool tcp_receive();
bool getTcpTwoConfigStatus();
void setTcpTwoConfigStatus(bool tcpTwoconfig);

void set_tcp_receive(bool tcpReceive);
bool get_tcp_receive();
bool TCP_Receive_Open();
bool extractCommandValue(const char *cmd);
#endif /* TCP_H_ */
