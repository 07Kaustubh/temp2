/*
 * error_logger.h
 *
 *  Created on: Dec 26, 2024
 *      Author: manya
 */
#ifndef INC_ERROR_LOGGER_H_
#define INC_ERROR_LOGGER_H_

#include "FLASH_MEM.h"
#include <stdbool.h>

// Constants
#define ERROR_LOG_PAGE_LIMIT  63   // Limit of pages in a block for error logging
#define ERROR_LOG_PAGE_SIZE   2048 // Size of a single page in bytes
#define ERROR_LOG_BLOCK       0   // Block number reserved for error logs
#define ERROR_LOG_PLANE       1    // Plane reserved for error logs

// Structure for error log data
typedef struct {
    char file[64];       // File name where the error occurred
    char function[64];   // Function name where the error occurred
    int line;            // Line number where the error occurred
    char message[128];   // Optional error message
} ErrorLog;

// Function declarations
bool logError(const char *file, const char *function, int line, const char *message);
unsigned long calculateFlashAddress(int page, int block, int plane);
void er_log();
//#define flash_printc(message, ...) \
//    if (printFlashConfig.printc_enabled ) \
//		logError(__FILE__, __func__, __LINE__, message)
//
//#define flash_print_d(message, ...) \
//    if (printFlashConfig.print_d_enabled ) \
//		logError(__FILE__, __func__, __LINE__, message)
//
//#define flash_print_e(message, ...) \
//    if (printFlashConfig.print_e_enabled ) \
//		logError(__FILE__, __func__, __LINE__, message)

#endif // INC_ERROR_LOGGER_H_
