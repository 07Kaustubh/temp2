/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash.c
  * @brief   This file provides code for the configuration
  *          of the flash instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* USER CODE BEGIN 0 */
uint32_t FirstSector, NbOfSectors, BankNumber, Address, SectorError;
__IO uint32_t MemoryProgramStatus = 0;

long StartAddress,                                   // START ADDRESS FLASH DATA
		SPIFlashAddress, FlashAddress;

unsigned int NOLs = 0, WrnData = 0;

extern char StatKeys[3][33];
extern char ComKeys[8][33];
extern char Imei[16];
extern L1,L2,L3;
int packet_size = 1024;
int Total_Packets =200;
 uint8_t FOTABuffer[1024];
FLASH_EraseInitTypeDef EraseInitStruct;  // Erase structure
/* USER CODE END 0 */

/* FLASH init function */
void MX_FLASH_Init(void)
{

  /* USER CODE BEGIN FLASH_Init 0 */

  /* USER CODE END FLASH_Init 0 */

  FLASH_OBProgramInitTypeDef pOBInit = {0};

  /* USER CODE BEGIN FLASH_Init 1 */

  /* USER CODE END FLASH_Init 1 */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Option Bytes settings */

  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

   //!!! HAL_FLASHEx_OBProgram is commented because some parameters are missing
  //pOBInit.OptionType = OPTIONBYTE_PROD_STATE|OPTIONBYTE_USER;
  //pOBInit.USERType = ;
  //pOBInit.USERConfig = ;
    //HAL_FLASHEx_OBProgram(&pOBInit);
  if (HAL_FLASH_OB_Lock() != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_FLASH_Lock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Launch Option Bytes Loading */
  /*HAL_FLASH_OB_Launch(); */

  /* USER CODE BEGIN FLASH_Init 2 */

  /* USER CODE END FLASH_Init 2 */

}

/* USER CODE BEGIN 1 */

uint32_t GetSector(uint32_t Address) {
	uint32_t sector = 0;

	if ((Address >= FLASH_BASE) && (Address < FLASH_BASE + 0x100000)) {
		sector = (Address & ~FLASH_BASE) / FLASH_SECTOR_SIZE;
	} else if ((Address >= FLASH_BASE + 0x100000)
			&& (Address < FLASH_BASE + 0x200000)) {
		sector =
				((Address & ~FLASH_BASE) - 0x100000) / FLASH_SECTOR_SIZE;
	} else {
		sector = 0xFFFFFFFF; /* Address out of range */
	}

	return sector;
}

/**
 * @brief  Gets the bank of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The bank of a given address
 */
uint32_t GetBank(uint32_t Addr) {
	uint32_t bank = 0;

	if (READ_BIT(FLASH->OPTSR_CUR, FLASH_OPTSR_SWAP_BANK) == 0) {
		/* No Bank swap */
		if (Addr < (FLASH_BASE + 0x100000)) {
			bank = FLASH_BANK_1;
		} else {
			bank = FLASH_BANK_2;
		}
	} else {
		/* Bank swap */
		if (Addr < (FLASH_BASE + 0x100000)) {
			bank = FLASH_BANK_2;
		} else {
			bank = FLASH_BANK_1;
		}
	}

	return bank;
}

/**
 * @brief  Check program operation.
 * param StartAddress Area start address
 * param EndAddress Area end address
 * param Data Expected data
 * @retval FailCounter
 */
uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress,
		uint32_t *Data) {
	uint32_t Address;
	uint32_t index, FailCounter = 0;
	uint32_t data32;

	/* check the user Flash area word by word */
	Address = StartAddress;

	while (Address < EndAddress) {
		for (index = 0; index < 4; index++) {
			data32 = *(uint32_t*) Address;
			if (data32 != Data[index]) {
				FailCounter++;
				return FailCounter;
			}
			Address += 4;
		}
	}
	return FailCounter;
}

void HAL_FLASH_ERASE(uint32_t FlashEraseAddress) {
	// uint32_t SectorError = 0, FirstSector = 0,NbOfSectors = 0, BankNumber = 0;

	FirstSector = GetSector(FlashEraseAddress);
	NbOfSectors = GetSector(FlashEraseAddress) - FirstSector + 1;
	BankNumber = GetBank(FlashEraseAddress);
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Banks = BankNumber;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		Error_Handler();
	}
	FLASH_WaitForLastOperation((uint32_t) FLASH_TIMEOUT_VALUE);
	HAL_FLASH_Lock();
}

bool HAL_FLASH_Write_Buff(uint32_t FlashAddress, uint8_t *Data,uint32_t DataSize) {
	uint32_t LoopCnt = DataSize / 16; //1+((DataSize -1)/ 16);// Calculate the number of sets of 4 quad-words
	uint64_t FlashData[2] = { 0 }; // Allocate memory for FlashData array
	// Unlock flash memory before writing
	HAL_FLASH_Unlock();

	for (int l = 0; l < LoopCnt; l++) {
		// Copy 8 bytes at a time from Data to FlashData
		for (int j = 0; j < 2; j++) {
			memcpy(&FlashData[j], &Data[((l * 16) + (j * 8))], 8);
		}
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, FlashAddress, FlashData) != 0x00 )
		{
			HAL_FLASH_Lock();
			return false;
		}
		// Increment address for the next set of quad-words
		FlashAddress += 16; // Increment by 32 bytes (4 quad-words)

	}

	// Lock flash memory after writing
	HAL_FLASH_Lock();
return true;
}


void Erase_sectors(uint32_t FlashAddress) {
	uint32_t start_sector_address = FlashAddress;
	uint32_t end_sector;

	// Determine the end sector based on FlashAddress
	if (FlashAddress == L2) {
		end_sector = start_sector_address + (31 * FLASH_SECTOR_SIZE); // 32 sectors
	} else if (FlashAddress == L3) {
		end_sector = start_sector_address + (31 * FLASH_SECTOR_SIZE); // 32 sectors
	} else {
		// Handle invalid FlashAddress
		Error_Handler();
		return;
	}
	// Iterate over sectors to erase
	for (uint32_t sector_address = start_sector_address;
			sector_address <= end_sector; sector_address += FLASH_SECTOR_SIZE) {
		HAL_FLASH_ERASE(sector_address); // Erase the sector
	}
}

void FLASH_Read_Buff(uint32_t Read_Address, char Read_Buff[], uint32_t ReadDataSize) {
    memset(Read_Buff, 0, ReadDataSize);
    for (uint32_t i = 0; i < ReadDataSize; i += 8) {
        uint64_t FlashData = (*(__IO uint64_t*) Read_Address);
        for (uint8_t j = 0; j < 8 && (i + j) < ReadDataSize; j++) {
            Read_Buff[i + j] = (char)((FlashData >> (j * 8)) & 0xFF);
        }
        Read_Address += 8;
    }
}

//bool WriteF_SKey() {
//	HAL_FLASH_ERASE(Config_Data);
//	uint32_t KAddr = S_Key_ADDR;
//	for (int a = 0; a < 3; a++) {
//		if(!HAL_FLASH_Write_Buff(KAddr, (uint32_t*) &StatKeys[a],
//				sizeof(StatKeys[a])))
//			return false;
//		KAddr += 32;
//		return true;
//	}
//	for (int a = 1; a <= 3; a++)
//		ReadF_Loc(1, a);
//}

//bool WriteF_CKey() {
//
//	uint32_t KAddr = C_Key_ADDR;
////		HAL_FLASH_ERASE(KAddr);
//	for (int a = 0; a < 8; a++) {
//		if(!HAL_FLASH_Write_Buff(KAddr, (uint32_t*) &ComKeys[a],
//				sizeof(ComKeys[a])))
//			return false;
//		KAddr += 32;
//	return true;
//	}
//	for (int a = 1; a <= 8; a++)
//		ReadF_Loc(2, a);
//
//}

void writeF_CP(uint64_t CData, uint64_t CData2, uint64_t DData3) {
	//
	HAL_FLASH_ERASE(Current_App_LOC);
	uint32_t KAddr = Current_App_LOC;
	uint32_t Kaddr2 = Current_App_LOC +16;
	uint32_t PHadr = Current_App_LOC +32;
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, KAddr, &CData);
	HAL_FLASH_Lock();
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Kaddr2, &CData2);
	HAL_FLASH_Lock();
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, PHadr, &DData3);
	HAL_FLASH_Lock();

}

//not needed to write imei to memory since if imei not possible to read something wrong with ec200
//void WriteF_IMEI(uint64_t *Value) {
//	// Write IMEI to Flash memory
//	HAL_FLASH_Unlock();
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Imei_Addr, &Value);
//	HAL_FLASH_Lock();
//}

//void WriteF_FOTAVer(uint64_t *Ver_buff) {
//	// Write the version buffer to Flash memory
//	HAL_FLASH_Unlock();
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Last_App_ver_Addr, Ver_buff);
//	HAL_FLASH_Lock();
//}

//void ReadF_sKey(int KeyNumber) {
//	FlashAddress = (uint32_t) S_Key_ADDR + (32 * (KeyNumber - 1));
//	uint8_t SKeyRbuff[32] = { 0 };
//
//	for (uint8_t i = 0; i < 32; i++) {
//		uint64_t FlashData = (*(__IO uint64_t*) FlashAddress);
//		for (uint8_t j = 0; j < 8; j++) {
//			SKeyRbuff[i + j] = ((FlashData >> (j * 8)) & 0xFF);
//		}
//		i += 7;
//		FlashAddress += 8;
//	}
//	// Print the contents of SKeyRbuff
////	    printf("Static Key %d: ", KeyNumber);
////	    for (uint8_t i = 0; i < 32; i++) {
////	        printf("%02X ", SKeyRbuff[i]); // Print each byte as hexadecimal
////	    }
////	    printf("\r\n");
//}




bool CopyFlashSectors(uint32_t sourceAddr, uint32_t destAddr, uint32_t size) {


	HAL_FLASH_Unlock();

	// Erase the destination sector
	Erase_sectors(destAddr);
	// Write data from the RAM buffer to the destination sector
	for (int i = 0; i < Total_Packets; i++) {
		memcpy(FOTABuffer, (uint8_t*) sourceAddr, packet_size);
//		FLASH_Read_Buff(sourceAddr, &flashBuffer, packet_size);
//		HAL_Delay(100);  // Adding delay to ensure proper timing
		if(!HAL_FLASH_Write_Buff(destAddr, &FOTABuffer,packet_size))
			return false;
		memset(FOTABuffer,0,sizeof(FOTABuffer));
		destAddr += 1024;
		sourceAddr +=1024;

	}
	printc(500,"DataCopied");
return true;
}

void Write_CFGFlash(uint32_t WriteLoc, uint64_t *DATA) {
	uint32_t KAddr = WriteLoc;
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, KAddr, DATA); // Corrected to pass address of WDT
	HAL_FLASH_Lock();
}

/* USER CODE END 1 */
