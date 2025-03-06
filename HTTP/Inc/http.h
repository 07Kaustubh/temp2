/*
 * http.h
 *
 *  Created on: Jun 3, 2024
 *      Author: saurabh
 *
 *      --------HTTP HANDLE-------
 *		FILE CHECK
 *		APN SET
 *		PDP ACTIVE
 *		INTERNET ENABLE (QIACT)
 *		HTTP CONFIG
 *		HTTP POST METHOD
 *		URL
 *		POSTFILE (PUSH FILE TO SERVER)
 */

#ifndef INC_HTTP_H_
#define INC_HTTP_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32h5xx_hal.h"
#include "EC200Utils.h"

extern UART_HandleTypeDef huart1;

#define INTOUCH_URL  "https://intouchdev.mapmyindia.com/IntouchFOTACOTA/"
#define EC_OUT  (int*)80

bool http_init();
bool http_ssl();
bool http_file_send(char*name);
bool http_post_data(const char *data,char *res,char*keyValue,int *);
//bool configureAPN(int contextID, int contextType);
//bool activatePDPContext(int contextID, int* contextState, int* contextType);
//bool deactivatePDPContext(int contextID);
//bool getHTTPConfig(int contextID, int *resultContextID);
//bool setOrGetHTTPContentType(int contextID, int* contentType);
//bool setOrGetHTTPFormData(int contextID, char *name, char *filename, char *contentType);
//bool setHttpUrl(int urlLength, int timeoutSeconds, const char* url);
//bool sendHttpPostFile(const char* filename, int rsptime, int postMode);
//bool cancelHttpRequest();
bool http_post_read_data(char *res,int timeout);
bool http_fota_read_file(char *resbuff,char *filename);
void set_http_SSL_Status(bool sslFlag);
bool get_http_SSL_Status();
bool http_fota_get(char *resbuff);
void set_http_Config_status(bool httpConfig);
bool get_http_Config_status();
/*********************************FILE RELATED THINGS***************************************/
bool file_delete(const char *fileNam);
uint16_t file_open(const char * fileName,int mode);
bool file_close(uint16_t filehandler);
bool file_read(uint16_t fileHandler,int length,char*resbuff);
bool file_read_non_it(uint16_t fileHandler,int length,char*resbuff);
bool file_seek(int fileHandler);
/*************************************************************************/

#endif /* INC_HTTP_H_ */
