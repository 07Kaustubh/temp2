/*
 * common.h
 *
 *  Created on: Feb 21, 2024
 *      Author: 91944
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_
//use this for set up uart in which you want to print the value
#include<stdio.h>
#include "stm32h5xx_hal.h"
#include<string.h>
#include <stdio.h>
#include <stdarg.h>
#include<stdbool.h>
#include "stdint.h"
#include "SPI_FLASH.h"
/********************ERROR LOG PARTS*********************************/

#define MinDelay 10
#define PRINT_RESBUFF_SIZE 500
#define FLASH_PAGE_SIZE 100  // Flash page size in bytes
#define __DIR__ parseLastDir(__FILE__)

typedef struct{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	char date[9];
	char time[7];
}Time;

typedef enum {
	PRINTC,
	PRINT_D,
	PRINT_E,
} PrintType;

typedef enum{
	IOCL,
	ER_LOG,
	TOTAL_MODULES
}Module_Type;

// Structure for folder-specific print configuration
typedef struct {
    const char* moduleName;  // Just the folder name, not full path
    bool isModuleEnable;
} ModuleConfig;

// Enhanced print configuration structure
typedef struct {
    ModuleConfig moduleConfigs[TOTAL_MODULES];
    bool printcEnabled;
    bool printDdEnabled;
    bool printEEnabled;
    bool modeUart;
    bool modeFlash;
} EnhancedPrintConfig;


void flushBufferToFlash();
void enableFlashWrites(bool enable);
void readLogsFromFlash();

const char* get_directory_name(const char* file_path);
bool should_print_for_folder(const char* file_path, PrintType type, const EnhancedPrintConfig* config);


//#define printc(delay, message, ...)print_info(PRINTC,__FILE__, __func__, __LINE__, delay, message, ##__VA_ARGS__)

#define print_d(delay, message, ...)print_info(PRINT_D,__FILE__, __func__, __LINE__, HAL_MAX_DELAY, message, ##__VA_ARGS__)

#define print_e(delay, message, ...)print_info(PRINT_E,__FILE__, __func__, __LINE__, HAL_MAX_DELAY, message, ##__VA_ARGS__)
/*****************************************************/
#define MinDelay 10
#define PRINT_RESBUFF_SIZE 500
void clearBuffer(char* buffer, size_t size);
void setupPrintUart(UART_HandleTypeDef *uart,bool debug);
void printc(int delay,const char* msg, ...);
void printhex(int delay,uint8_t* data,int size);
void print(int delay,uint8_t* data,int size);
//bool splitString(char *string,char *spliter,char opbuffer[][100],uint8_t splitSize);
void openPrintUartIdle();
void print_RxEventCallback(uint16_t size);


void clearBuffer(char* buffer, size_t size);
bool splitString(char *string,char *spliter,char opbuffer[][100],uint8_t splitSize);
#endif /* INC_COMMON_H_ */
