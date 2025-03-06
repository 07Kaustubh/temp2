#ifndef  __SPI_FLASH_H
#define  __SPI_FLASH_H


#include"SPI_FLASH.h"
#include "common.h"
#include<stdbool.h>
/****************************************Nripender Function*******************************/
#define CONFIG_BUFFER_SIZE 50

#define NO_PAGE_IN_BLOCK 64
#define NO_BLOCK_IN_PLANE 1024
#define NO_PLANE 2
#define SIZE_OF_PAGE 2048
void setUpNANDFlash();
void writeConfigPage();
void makeConfigData(uint8_t *configBuff);
bool WriteInToNANDFLASH(uint8_t *pBuffer,unsigned long address,unsigned short  size);
bool deleteL86Data();
bool readL86Data(uint8_t *L89BufferData,int size);
bool writeL86Data(uint8_t *dataBuffer,int size);
bool RdN_Block();
/*****************************************************************************************/
#endif
