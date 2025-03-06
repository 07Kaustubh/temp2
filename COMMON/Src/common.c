/*
 * common.c
 *
 *  Created on: Feb 21, 2024
 *      Author: 91944
 */
#include "common.h"

UART_HandleTypeDef *printUart;
int printUartSet=0;
bool enablePrintUartIdle=true;
uint8_t printUart_ResponseBuff[PRINT_RESBUFF_SIZE]={};
uint8_t dataToSend[2048]={};
void setupPrintUart(UART_HandleTypeDef *uart,bool debug){
	printUart=uart;
	if(debug)
		printUartSet=1;
		openPrintUartIdle();
}
void printc(int delay,const char* msg, ...) {
	if(printUartSet){
		char message[2024];
		va_list args;
		va_start(args, msg);
		vsnprintf(message, sizeof(message), msg, args);
		va_end(args);
		HAL_UART_Transmit(printUart, (uint8_t*)message, strlen(message), delay);
	}
}
void print(int delay,uint8_t* data,int size){
	if(printUartSet){
		HAL_UART_Transmit(printUart,data,size, delay);
	}
}
void clearBuffer(char* buffer, size_t size) {
	// Use memset to fill the buffer with zeros
	memset(buffer, 0, size);
}
void printhex(int delay,uint8_t* data,int size){
	if(printUartSet){
		for (int i = 0; i < size; i++) {
			char hex[4];  // Buffer to store the hexadecimal representation of each byte
			sprintf(hex, "%02X ", data[i]);  // Format each byte as a two-digit hexadecimal number
			HAL_UART_Transmit(printUart, (uint8_t*)hex, strlen(hex), delay);
		}
	}
}

bool splitString(char *string,char *spliter,char opbuffer[][100],uint8_t splitSize){
	char *line=strstr(string,spliter);
	int i=0;
	if(line==NULL){
		memcpy(opbuffer[i++],string,strlen(string));
	}
	while(line!=NULL&&i<=splitSize){
		uint8_t length = line - string;
		char substring[length];
		memset(substring,0,length);
		if (substring == '\0') {
			printc(MinDelay,"Memory allocation failed\n");
			return false;
		}
		memcpy(substring,string,length);
		string=line+strlen(spliter);
		//		printc(MinDelay,"sub string \n %s\n",substring);
		if(substring=='\0')
			return false;
		if(i+1<splitSize)
			memcpy((uint8_t*)opbuffer[i++],substring,length);
		line=strstr(string,spliter);
		if(line==NULL){
			//			printc(MinDelay,"last line %s\n",string);
			if(i+1 < splitSize)
				memcpy(opbuffer[i++],string,strlen(string));
		}
	}
	return true;
}
void switchPrintIdle()
{
	if(enablePrintUartIdle)
		enablePrintUartIdle = false;
	else
		enablePrintUartIdle = true;
}
void openPrintUartIdle(){
		if(enablePrintUartIdle)
	HAL_UARTEx_ReceiveToIdle_IT(printUart,printUart_ResponseBuff,PRINT_RESBUFF_SIZE);
}
void print_RxEventCallback(uint16_t size){
	printc(MinDelay,"Callback From Print Uart %d %s\n",size,printUart_ResponseBuff);
	parseData(printUart_ResponseBuff,dataToSend);
	print(2000,dataToSend,strlen(dataToSend));
	onPrintUartCmd(printUart_ResponseBuff);
	clearBuffer(printUart_ResponseBuff,PRINT_RESBUFF_SIZE);
}
void setFlagDirect(uint8_t* flagStorage, uint8_t flag, bool value) {
    if (flagStorage == NULL) {
        printf("Invalid pointer!\n");
        return;
    }
    if (value) {
        *flagStorage |= (1 << flag);
    } else {
        *flagStorage &= ~(1 << flag);
    }
}
bool getFlagDirect(uint8_t* flagStorage, uint8_t flag) {
    if (flagStorage == NULL) {
        printf("Invalid pointer!\n");
        return false;
    }

    return (*flagStorage & (1 << flag)) != 0;
}





/*ERROR LOG INITIALIZATION AND DEFINES*/
EnhancedPrintConfig *printConfig;
FLASH_AddressTypeDef Log_Address={.Block=0,.Page=0,.Plane=1};
static uint8_t flashBuffer[FLASH_PAGE_SIZE];
static size_t bufferOffset = 0;
static bool flashWritesEnabled = true;

/**************ERROR LOG FUNCTIONS***************/
const char* parseSecondLastDir(const char *path) {
    static char folder_name[10];  // Buffer to store the result

    // Start from the end of the path
    const char *last_sep = strrchr(path, '/');
    if (!last_sep) last_sep = strrchr(path, '\\'); // Support backslashes on Windows

    if (!last_sep || last_sep == path) {
        return ""; // No directory structure found
    }

    // Move backwards to find the second-last separator
    const char *second_last_sep = last_sep - 1;
    while (second_last_sep > path && *second_last_sep != '/' && *second_last_sep != '\\') {
        second_last_sep--;
    }

    // Move backwards to find the third-last separator
    const char *third_last_sep = second_last_sep - 1;
    while (third_last_sep > path && *third_last_sep != '/' && *third_last_sep != '\\') {
        third_last_sep--;
    }

    if (third_last_sep == path && *third_last_sep != '/' && *third_last_sep != '\\') {
        return ""; // No third-last separator found
    }

    // Extract the second-last directory name
    size_t len = second_last_sep - third_last_sep - 1;
    if (len >= sizeof(folder_name)) {
        len = sizeof(folder_name) - 1; // Prevent buffer overflow
    }

    strncpy(folder_name, third_last_sep + 1, len);
    folder_name[len] = '\0';

    return folder_name;
}



bool should_print_for_folder(const char* filePath, PrintType type, const EnhancedPrintConfig* config) {
   // const char* current_dir = get_directory_name(file_path);
    const char* current_dir = parseSecondLastDir(filePath);
    bool result = false;
    // First check if there's a specific configuration for this folder
    for(size_t i = 0; i < TOTAL_MODULES; i++) {
        if( config->moduleConfigs[i].isModuleEnable == true) {
          if( strcmp(current_dir,config->moduleConfigs[i].moduleName)==0 ){
        	  result = true;
        	  break;
          }
        }
    }
return result;

}

// Print message in the specified format
void print_info(PrintType type,const char *file, const char *func, int line, uint8_t delay, const char *msg, ...) {
		if (!printUartSet)  return;

		// Check if we should print for this file's folder
		if (!should_print_for_folder(file, type, printConfig)) return;


        char formatted_msg[1024];  // Buffer for the user message
        char final_msg[2024];      // Buffer for the complete message
        Time currentTime;
        getDateTime(&currentTime);

        // Format the user message
        va_list args;
        va_start(args, msg);
        vsnprintf(formatted_msg, sizeof(formatted_msg), msg, args);
        va_end(args);

        // Add metadata to the formatted message
        snprintf(final_msg, sizeof(final_msg), "[%s %s] [%s@%s:%d] %s\r\n",currentTime.date, currentTime.time, func, file, line, formatted_msg);

        if(printConfig->modeUart == true){
          if( (printConfig->printDdEnabled==true && type == PRINT_D) ||
        	  (printConfig->printEEnabled==true && type == PRINT_E) ||
			  (printConfig->printcEnabled==true && type == PRINTC))

        	  HAL_UART_Transmit(printUart, (uint8_t *)final_msg, strlen(final_msg), delay);
        }

        if(printConfig->modeFlash == true && flashWritesEnabled == true){
        	if( (printConfig->printDdEnabled==true && type == PRINT_D) ||
              	  (printConfig->printEEnabled==true && type == PRINT_E) ||
      			  (printConfig->printcEnabled==true && type == PRINTC))

//        		{		size_t remainingSpace = FLASH_PAGE_SIZE - bufferOffset;
//        		        size_t messageLength = strlen(final_msg);
//
//        		        if (messageLength > remainingSpace) {
//        		            flushBufferToFlash();
//        		        }
//        		        memcpy(&flashBuffer[bufferOffset], final_msg, messageLength);
//        		        bufferOffset += messageLength;
//        		}

        	{
        		size_t messageLength = strlen(final_msg);
        		const char *data = final_msg;
        		while (messageLength > 0) {
        		        // Calculate remaining space in the buffer
        		        size_t remainingSpace = FLASH_PAGE_SIZE - bufferOffset;

        		        if (messageLength <= remainingSpace) {
        		            // Message fits in the current buffer
        		            memcpy(&flashBuffer[bufferOffset], data, messageLength);
        		            bufferOffset += messageLength;
        		            messageLength = 0; // All data written
        		        } else {
        		            // Fill the current buffer and flush
        		            memcpy(&flashBuffer[bufferOffset], data, remainingSpace);
        		            bufferOffset = FLASH_PAGE_SIZE;
        		            flushBufferToFlash();

        		            // Adjust pointers for the remaining data
        		            data += remainingSpace;
        		            messageLength -= remainingSpace;
        		        }

        		        // If buffer becomes full during processing, flush it
        		        if (bufferOffset == FLASH_PAGE_SIZE) {
        		            flushBufferToFlash();
        		        }

        		    }
        	}

//        	{
//        		size_t messageLength = strlen(final_msg);
//        		        size_t remainingSpace = FLASH_PAGE_SIZE - bufferOffset;
//
//        		        if (messageLength <= remainingSpace) {
//        		            // Message fits in current page
//        		            memcpy(&flashBuffer[bufferOffset], final_msg, messageLength);
//        		            bufferOffset += messageLength;
//        		        } else {
//        		            // Message needs to be split across pages
//        		            // First, fill the current page
//        		            memcpy(&flashBuffer[bufferOffset], final_msg, remainingSpace);
//
//        		            // Write current page to flash
//        		            flushBufferToFlash();
//
//        		            // Now handle remaining data
//        		            size_t remainingData = messageLength - remainingSpace;
//        		            size_t currentOffset = remainingSpace;
//
//        		            // Write full pages if needed
//        		            while (remainingData > FLASH_PAGE_SIZE) {
//        		                memcpy(flashBuffer, &final_msg[currentOffset], FLASH_PAGE_SIZE);
//        		                bufferOffset = FLASH_PAGE_SIZE;
//        		                flushBufferToFlash();
//        		                remainingData -= FLASH_PAGE_SIZE;
//        		                currentOffset += FLASH_PAGE_SIZE;
//        		            }
//
//        		            // Write final partial page if any data remains
//        		            if (remainingData > 0) {
//        		                memcpy(flashBuffer, &final_msg[currentOffset], remainingData);
//        		                bufferOffset = remainingData;
//        		            }
//        		        }
//
//        		        // If buffer is full, flush it
//        		        if (bufferOffset == FLASH_PAGE_SIZE) {
//        		            flushBufferToFlash();
//        		        }
//        	}

           }
}

void flushBufferToFlash() {
	if (bufferOffset == 0) return;

	// If we're at the first page of a block, erase the block
	if (Log_Address.Page == 0) {
		Unlock_ALL_Blocks();
		Block_Lock_Protection_Disable();
		Block_Erase(calculateFlashAddress(Log_Address.Page, Log_Address.Block, Log_Address.Plane));
	}
	Unlock_ALL_Blocks();
	Block_Lock_Protection_Disable();
	//Log_Address.Page++;
	WriteInToNANDFLASH(flashBuffer, calculateFlashAddress(Log_Address.Page, Log_Address.Block, Log_Address.Plane), bufferOffset);
	memset(flashBuffer, 0, FLASH_PAGE_SIZE);
	bufferOffset = 0;

	Log_Address.Page++;
	if (Log_Address.Page > 63) {
		Log_Address.Block++;
		Log_Address.Page = 0;
	}
	if (Log_Address.Block > 1023){
		Block_Erase(0);
		Log_Address.Block = 0;
		Log_Address.Page = 0;}
}

void enableFlashWrites(bool enable) {
	flashWritesEnabled = enable;
}

void readLogsFromFlash() {
	enableFlashWrites(false);
	FLASH_AddressTypeDef readAddress = { .Block = 0, .Page = 0, .Plane = 1 };
	uint8_t buffer[FLASH_PAGE_SIZE];

	while (readAddress.Block < Log_Address.Block ||
		   (readAddress.Block == Log_Address.Block && readAddress.Page <= Log_Address.Page)) {

		ReadFromNANDFLASH(buffer, calculateFlashAddress(readAddress.Page, readAddress.Block, readAddress.Plane), FLASH_PAGE_SIZE);
		print(MinDelay, buffer, FLASH_PAGE_SIZE);

		readAddress.Page++;
		if (readAddress.Page > 63) {
			readAddress.Block++;
			readAddress.Page = 0;
		}
	}
	enableFlashWrites(true);
}
