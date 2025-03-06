///*
// * error_logger.c
// *
// *  Created on: Dec 26, 2024
// *      Author: manya
// */
//#include "error_logger.h"
//#include <string.h>
//
//void er_log(){
//	// Test error logging
//		printc(MinDelay, "this is a debug message in er log file");
//		print_d(HAL_MAX_DELAY, "This is detailed information in er log file");
//		print_e(HAL_MAX_DELAY, "This is an error message in er log file");
//}
//
//
//FLASH_AddressTypeDef errorLog_Write_Address = {0, ERROR_LOG_BLOCK, ERROR_LOG_PLANE};
//uint64_t errorLog_Page = 0, errorLog_Block = ERROR_LOG_BLOCK;
//
//// Function to calculate Flash address
//unsigned long calculateFlashAddress(int page, int block, int plane) {
//    return SPI_FLASH_Creat_ADDR_TO_WRITE(page, block, plane);
//}
//
//// Function to log error data
//bool logError(const char *file, const char *function, int line, const char *message) {
//    ErrorLog errorLog;
//    memset(&errorLog, 0, sizeof(ErrorLog));
//
//    // Populate the error log structure
//    strncpy(errorLog.file, file, sizeof(errorLog.file) - 1);
//    strncpy(errorLog.function, function, sizeof(errorLog.function) - 1);
//    errorLog.line = line;
//    strncpy(errorLog.message, message, sizeof(errorLog.message) - 1);
//
////    // Ensure error log size fits within a page
////    if (sizeof(ErrorLog) > ERROR_LOG_PAGE_SIZE) {
////        printc(MinDelay, "Error log size exceeds page size");
////        return false;
////    }
////
//    // If we're at the first page of a block, erase the block
//    if (errorLog_Write_Address.Page == 0) {
//        Unlock_ALL_Blocks();
//        Block_Lock_Protection_Disable();
//        Block_Erase(errorLog_Write_Address.Block);
//    }
//
//    	Unlock_ALL_Blocks();
//    	Block_Lock_Protection_Disable();
//    	//Block_Erase(errorLog_Write_Address.Block);
//
//    // Write the error log to Flash memory
//    bool status = WriteInToNANDFLASH((uint8_t*)&errorLog,
//        calculateFlashAddress(errorLog_Write_Address.Page, errorLog_Write_Address.Block, errorLog_Write_Address.Plane),
//        sizeof(ErrorLog));
//
//    // Update the write address for the next error
//    errorLog_Write_Address.Page++;
//    if (errorLog_Write_Address.Page >= ERROR_LOG_PAGE_LIMIT) {
//    	errorLog_Write_Address.Block++;
//    	errorLog_Write_Address.Page = 0;
//
//    }
//
//    if (errorLog_Write_Address.Block >= 1023){
//    	Block_Erase(0);
//    	errorLog_Write_Address.Block = 0;
//    	errorLog_Write_Address.Page = 0;
//    }
//
//    return status;
//}
//
//
////testing read,write into nand flash
////	FLASH_AddressTypeDef Write_Address, Read_Address;
////	char flash_buffer[12] = "hello world";
////	Unlock_ALL_Blocks();
////	Block_Lock_Protection_Disable();
////	Block_Erase(0);
////	WriteInToNANDFLASH(flash_buffer,
////				caculateAddress(0, 0, 0), 12);
////	//printc(MinDelay, "\n Write Address \n%d", Write_Address.Page);
////	//	(Write_Address.Page)++;
////	clearBuffer(flash_buffer, 12);
////	ReadFromNANDFLASH(flash_buffer,
////					caculateAddress(0, 0, 0), 12);
//
//
////	             {  // If we're at the first page of a block, erase the block
////        		    if (Log_Address.Page == 0) {
////        		        Unlock_ALL_Blocks();
////        		        Block_Lock_Protection_Disable();
////        		        Block_Erase(Log_Address.Block);
////        		    }
////        		    	Unlock_ALL_Blocks();
////        		    	Block_Lock_Protection_Disable();
////        		    	//Block_Erase(Log_Address.Block);
////        		    // Write the error log to Flash memory
////        		    WriteInToNANDFLASH(formatted_msg,
////        		        calculateFlashAddress(Log_Address.Page, Log_Address.Block, Log_Address.Plane),
////        		        sizeof(formatted_msg));
////        		  //  clearBuffer(formatted_msg,1024);
////        		    // Update the write address for the next error
////        		    Log_Address.Page++;
////        		    if (Log_Address.Page > 63) {
////        		    	Log_Address.Block++;
////        		    	Log_Address.Page = 0;}
////        		    if (Log_Address.Block > 1024){
////        		    	Block_Erase(0);
////        		    	Log_Address.Block = 0;
////        		    	Log_Address.Page = 0;}
////        	        }
