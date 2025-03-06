/* Includes ------------------------------------------------------------------*/
#include "SPI_FLASH.h"

unsigned long Flash_Address, Temp_Address;

uint8_t Status;

uint16_t Flash_Time_Out, Unique_ID, Index, Config_Index;

uint32_t PAGE, BLOCK, Read_Address;

char Write_Buffer[2048] = { 0 }, Read_Buffer[4095] = { 0 }, Local_Buffer[2025],
		Status_Bit[8];

FLASH_AddressTypeDef Write_Address;

void SPI_FLASH_Reset_Write_ADD(void) {
	Write_Address.Page = INIT_WRITE_PAGE; /*INITIALIZE THE PAGE TO WRITE*/
	Write_Address.Block = INIT_WRITE_BLOCK; /*INITIALIZE THE BLOCK TO WRITE*/
	Write_Address.Plane = INIT_WRITE_PLANE; /*INITIALIZE THE PLANE TO WRITE*/

	Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(Write_Address.Page,
			Write_Address.Block, Write_Address.Plane); /*CREATE THE ADDRESS*/
	Block_Erase(Temp_Address);

}

void Unlock_ALL_Blocks(void) {
	SPI_FLASH_WriteEnable(); /* Send write enable Instruction */
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(SET_FEATURES); /* SET-Features */
	SPI_FLASH_SendByte(BLOCK_LOCK_REGISTER); /* Feature Address */
	SPI_FLASH_SendByte(0x00); /* Data-Bits values */
	HAL_Delay(1); /* Wait for 1ms */

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	SPI_FLASH_WriteDisable(); /* Send write Disable Instruction */
	HAL_Delay(1); /* Wait for 1ms */

	SPI_FLASH_Read_Block_Lock_Register(); /*READ THE STATUS OF BLOCK LOCK REGISTER*/

}

void SPI_FLASH_Write_ECC_Bit(uint8_t ECC_Mode) {
	SPI_FLASH_WriteEnable(); /* Send write enable Instruction */
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(SET_FEATURES); /* SET-Features */
	SPI_FLASH_SendByte(CONFIG_REGISTER); /* Feature Address */

	SPI_FLASH_SendByte(ECC_Mode); /*ECC MODE - SET OR RESET*/
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	SPI_FLASH_WriteDisable(); /* Send write Disable Instruction */

	SPI_FLASH_Check_ECC_Mode(); /*CHECK THE STATUS OF THE ECC MODE*/

}

void SPI_FLASH_Check_ECC_Mode(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	SPI_FLASH_SendByte(GET_FEATURES); /* GET-Features */
	SPI_FLASH_SendByte(CONFIG_REGISTER); /* Feature Address */

	Status = SPI_FLASH_SendByte(DUMMY_BYTE); /* TAKE STATUS BY SENDING DUMMY BYTE*/
	/*Check 4th bit for ECC*/

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
}

void SPI_FLASH_Read_Block_Lock_Register(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	SPI_FLASH_SendByte(GET_FEATURES); /* GET-Features */
	SPI_FLASH_SendByte(BLOCK_LOCK_REGISTER); /* READ BLOCK LOCK REGISTER*/

	Status = SPI_FLASH_SendByte(DUMMY_BYTE);
	FLASH_DISABLE;
}

void Parameter_Page(void) {
	SPI_FLASH_WriteEnable();
	FLASH_ENABLE;

	SPI_FLASH_SendByte(SET_FEATURES);
	SPI_FLASH_SendByte(CONFIG_REGISTER);

	SPI_FLASH_SendByte(0x40);

	Read_Page(Read_Buffer, 0X01, 255);

	SPI_FLASH_SendByte(SET_FEATURES);
	SPI_FLASH_SendByte(CONFIG_REGISTER);

	SPI_FLASH_SendByte(0x00);

	SPI_FLASH_WriteDisable();
	FLASH_DISABLE;
}

void Block_Lock_Protection_Disable(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(SET_FEATURES); /* SET-Features */
	SPI_FLASH_SendByte(CONFIG_REGISTER); /* Feature Address */

	SPI_FLASH_SendByte(0xC2); /* Data-Bits values */

	SPI_FLASH_WriteEnable();
	HAL_Delay(1);
	FLASH_ENABLE;
	SPI_FLASH_SendByte(0x10);
	HAL_Delay(1);
	uint8_t Status = 1;
	uint32_t start_time = HAL_GetTick();                 // Get the current time
	while (Status != 0 && HAL_GetTick() - start_time < TIMEOUT_MS) {
		Status = read_status_register();
	}

	FLASH_DISABLE;
}

unsigned char SPI_FLASH_SendByte(unsigned char byte) {
	/*!< Loop while DR register in not emplty */
	while (LL_SPI_IsActiveFlag_TXP(SPI3) == RESET)
		;

	/*!< Send byte through the SPI3 peripheral */
	LL_SPI_TransmitData8(SPI3, byte);

	/*!< Wait to receive a byte */
	while (LL_SPI_IsActiveFlag_RXP(SPI3) == RESET)
		;

	/*!< Recieve & !< Return the byte read from the SPI bus */
	return LL_SPI_ReceiveData8(SPI3);
}

void SPI_FLASH_ReadID(void) // Flash ID values Return
{
	char ID_Buff[3] = { 0 }; /*Initialize local variable*/

	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(READ_ID_CMD); /* Send "RDID " instruction */
	SPI_FLASH_SendByte(DUMMY_BYTE); /* Send One Dummy Byte */

	ID_Buff[0] = SPI_FLASH_SendByte(DUMMY_BYTE); /* receive-manufaturer-ID (1-byte) by sending Nothing || Dummy Byte */
	ID_Buff[1] = SPI_FLASH_SendByte(DUMMY_BYTE); /* receive-device-ID (1-byte) by sending Nothing || Dummy Byte */

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */

	Unique_ID = (ID_Buff[0] << 8) | ID_Buff[1]; /* Combine Both ID's to Make Unique_ID */

}

void SPI_FLASH_WriteEnable(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	HAL_Delay(1); /* Wait for 1ms */

	SPI_FLASH_SendByte(WRITE_EN_CMD); /* Send "Write Enable" Instruction */

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
}

void SPI_FLASH_WriteDisable(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	HAL_Delay(1); /* Wait for 1ms */

	SPI_FLASH_SendByte(WRITE_DIS_CMD); /* Send "Write Enable" instruction */

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
}

void Block_Erase(unsigned long Block_Addr) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	SPI_FLASH_SendByte(0xFF); // asked by riya to add this
	FLASH_DISABLE;
	HAL_Delay(1);

	FLASH_ENABLE;
	SPI_FLASH_SendByte(WRITE_EN_CMD); /* Send "Write Enable" Instruction */
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	HAL_Delay(1); /* Wait for 1ms */

	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	//SPI_FLASH_Check_ECC_Mode();

	SPI_FLASH_SendByte(BLOCK_ERASE); /* Send Bulk Erase Instruction */

	SPI_FLASH_SendByte((Block_Addr & 0xFF0000) >> 16); /* Send Block_Addr high nibble address byte *//* Dummy(8-Bits) */
	SPI_FLASH_SendByte((Block_Addr & 0xFF00) >> 8); /* Send Block_Addr medium nibble address byte *//* Block Address */
	SPI_FLASH_SendByte(Block_Addr & 0xFF); /* Send Block_Addr low nibble address byte *//* Page Address  */

	HAL_Delay(1);
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	HAL_Delay(1); /* Wait for 3ms - Minimum it takes 2ms to erase the Block */

	uint8_t Stat = 1;
	uint32_t start_time = HAL_GetTick();                 // Get the current time
	while (Stat != 0 && HAL_GetTick() - start_time < TIMEOUT_MS) {
		Stat = read_status_register();
	}

	FLASH_ENABLE;
	SPI_FLASH_SendByte(WRITE_DIS_CMD); /* Send "Write Enable" instruction */
	FLASH_DISABLE;

	SPI_FLASH_Write_ECC_Bit(1);

}

void Flash_EN() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}
void Flash_Dis() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

uint8_t read_status_register() {
	int a = 0;
	Status = 0;
	FLASH_ENABLE; // Enable the flash
	SPI_FLASH_SendByte(GET_FEATURES);        // Send Get Features command
	SPI_FLASH_SendByte(STATUS_REG_ADDR);     // Send status register address

	Status = SPI_FLASH_SendByte(DUMMY_BYTE);
	for (int i = 0; i < 8; i++) {
		Status_Bit[i] = (Status & (1 << i)) != 0;
	}

	if (Status_Bit[0] == 1)
		a = 1;

	if (Status_Bit[1] == 1)
		a = 2;

	if (Status_Bit[2] == 1)
		a = 3;

	if (Status_Bit[3] == 1)
		a = 4;

	if ((Status_Bit[4] == 1) || (Status_Bit[5] == 1) || (Status_Bit[6] == 1))
		a = 567;
	if (Status_Bit[7] == 1)
		a = 8;
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */

	return a; // Return the status byte
}
uint8_t Flash_write(char *pBuffer, unsigned long Write_Addr,
		unsigned short Total_Bytes_To_Write) {

	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	SPI_FLASH_SendByte(WRITE_EN_CMD); /* Send "Write Enable" Instruction */
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10); /* Wait for 1ms */
	FLASH_ENABLE; /* Enable the write access to the FLASH */
	SPI_FLASH_SendByte(PROGRAM_LOAD);
	SPI_FLASH_SendByte(DUMMY_BYTE);
	SPI_FLASH_SendByte(DUMMY_BYTE);
	/* Send CMD To Load Program in the Flash */
	while (Total_Bytes_To_Write--) /* while there is data to be written on the FLASH */
	{
		SPI_FLASH_SendByte(*pBuffer); /* Send the byte One By one From The Buffer */
		pBuffer++; /* Point on the next byte to be written */
	}

	HAL_Delay(10); /* Wait for 5ms */
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10);
	FLASH_ENABLE; /* Enable the write access to the FLASH */

	SPI_FLASH_SendByte(PROGRAM_EXECUTE); /* Send Program-Execute CMD */

	SPI_FLASH_SendByte((Write_Addr & 0xFF0000) >> 16); /* Send Read_Addr high nibble address byte *//* Dummy(8-Bits) */
	SPI_FLASH_SendByte((Write_Addr & 0xFF00) >> 8); /* Send Read_Addr medium nibble address byte *//* Block Address */
	SPI_FLASH_SendByte(Write_Addr & 0xFF); /* Send Read_Addr low nibble address byte *//* Page Address  */

	HAL_Delay(10);
	FLASH_DISABLE;/* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10);

	uint8_t Status = 1;
	uint32_t start_time = HAL_GetTick();                 // Get the current time
	while (Status != 0 && HAL_GetTick() - start_time < TIMEOUT_MS) {
		Status = read_status_register();
	}
	FLASH_ENABLE;
	SPI_FLASH_SendByte(WRITE_DIS_CMD); /* Send "Write Enable" instruction */
	FLASH_DISABLE;
	/* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10);
	if (Status == 0)
		return true;
	return Status;
}

uint8_t Read_Page(char *pBuffer, unsigned long Read_Addr,
		unsigned short Total_Bytes_To_Read) {
	HAL_Delay(10);
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(PAGE_READ_CMD); /* Send "Read from Memory " Instruction */

	SPI_FLASH_SendByte((Read_Addr & 0xFF0000) >> 16); /* Send Read_Addr high nibble address byte *//* Dummy(8-Bits) */
	SPI_FLASH_SendByte((Read_Addr & 0xFF00) >> 8); /* Send Read_Addr medium nibble address byte *//* Block Address */
	SPI_FLASH_SendByte(Read_Addr & 0xFF); /* Send Read_Addr low nibble address byte *//* Page Address  */

	HAL_Delay(10);
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10); /* Wait for 1ms */

	uint8_t Status = 1;
	uint32_t start_time = HAL_GetTick();                 // Get the current time
	while (Status != 0 && HAL_GetTick() - start_time < TIMEOUT_MS) {
		Status = read_status_register();
	}
	/* DISABLE the FLASH: Chip Select high */
	HAL_Delay(10); /* Wait for 1ms */
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */

	SPI_FLASH_SendByte(READ_FROM_CACHE); /* Send the CMD to Read from Cache 1 */

	// These Dummy byte are need to be send for correct or alinged data.
	SPI_FLASH_SendByte(DUMMY_BYTE);
	SPI_FLASH_SendByte(DUMMY_BYTE);

	while (Total_Bytes_To_Read--) /* while there is data to be read */
	{
		*pBuffer = SPI_FLASH_SendByte(DUMMY_BYTE); /* Read a byte from the FLASH By Sending Dummy, So that IC can have the CLOCK*/
		pBuffer++; /* Point to the next location where the byte read will be saved */
	}

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */
	return Status;
}

/*******************************************************************************
 * Function Name  : SPI_FLASH_Increament_Flash_Addr
 * Description    : Increment the Flash Page,Block & plane.
 * Input          : None
 * Return         : None
 *******************************************************************************/
void SPI_FLASH_Increament_Flash_Addr(void) {
	(Write_Address.Page)++; /*INCREAMENT THE PAGE NO.*/

	if (Write_Address.Page > WRITE_PAGES_LIMIT) {
		Write_Address.Page = INIT_WRITE_PAGE; /* RESET THE PAGE NO.*/
		(Write_Address.Block)++; /* INCREAMENT THE BLOCK NO.*/
	}

	if (Write_Address.Block > LAST_BLOCK_ADDR) {
		Write_Address.Page = INIT_WRITE_PAGE; /* RESET THE PAGE NO.*/
		Write_Address.Block = INIT_WRITE_BLOCK; /* RESET THE BLOCK NO.*/
		(Write_Address.Plane)++; /* INCREAMENT THE PLANE NO.*/
	}

	if (Write_Address.Plane > MAX_PLANE_AVAILABLE) {
		SPI_FLASH_Reset_Write_ADD(); /*RESET THE PAGE, BLOCK & PLANE IF REACHED LIMIT*/
	}
}

void SPI_FLASH_Full_Erase(void) {
	FLASH_ENABLE; /* ENABLE the FLASH: Chip Select low */
	SPI_FLASH_SendByte(WRITE_EN_CMD); /* Send "Write Enable" Instruction */
	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */

	HAL_Delay(1);
	FLASH_ENABLE;
	SPI_FLASH_SendByte(CHIP_ERASE); /* Send Bulk Erase instruction */

	FLASH_DISABLE; /* DISABLE the FLASH: Chip Select high */

	SPI_FLASH_WriteDisable(); /*Send Write Disable instruction*/
}

/*******************************************************************************
 * Function Name  : SPI_FLASH_Creat_ADDR_TO_WRITE
 * Description    : CREATE THE ADDRESS TO WRITE WITH COMBINATION OF PAGE AND BLOCK
 * Input          : None
 * Return         : unsigned int - RETURN THE ADDRESS
 *******************************************************************************/
unsigned long SPI_FLASH_Creat_ADDR_TO_WRITE(unsigned int Page_Addr,
		unsigned int Block_Addr, unsigned int Plane_Addr) {
	unsigned long TEMP_ADDR = 0;

	TEMP_ADDR |= (Page_Addr & 0x3F);                // Bits 0 to 5
	TEMP_ADDR |= ((Plane_Addr << 6) & 0x40);         // Bit 6
	TEMP_ADDR |= ((Block_Addr << 7) & 0x1FF80);      // Bits 7 to 16

	return TEMP_ADDR;
}

//void Bulk_Write(char *pBuffer, unsigned long Write_Addr,
//		unsigned short Total_Bytes_To_Write) {
//	unsigned char NumOfPage = 0;
//	unsigned short Remaining_Bytes = 0;
//
//	Temp_Address = Write_Addr;
//	NumOfPage = Total_Bytes_To_Write / SPI_FLASH_PageSize;
//
//	if (Write_Address.Page == INIT_WRITE_PAGE) {
//		Block_Erase(Flash_Address);
//		HAL_Delay(10);
//	}
//
//	if (NumOfPage == 0) /* Total_Bytes_To_Write < SPI_FLASH_PageSize */
//	{
//		Flash_write(pBuffer, Temp_Address, Total_Bytes_To_Write);
//	} else /* Total_Bytes_To_Write > SPI_FLASH_PageSize */
//	{
//		while (NumOfPage--) {
//			Flash_write(pBuffer, Temp_Address, SPI_FLASH_PageSize);
//
//			SPI_FLASH_Increament_Flash_Addr();
//			if (Write_Address.Page == INIT_WRITE_PAGE) {
//				Block_Erase(Flash_Address);
//				HAL_Delay(10);
//			}
//
//			Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(Write_Address.Page,
//					Write_Address.Block, Write_Address.Plane);
//
//			pBuffer += SPI_FLASH_PageSize;
//
//			Remaining_Bytes = Total_Bytes_To_Write = Total_Bytes_To_Write
//					- SPI_FLASH_PageSize;
//			if (Remaining_Bytes >= SPI_FLASH_PageSize)
//				continue;
//			else
//				break;
//		}
//		if (Remaining_Bytes != 0) {
//			SPI_FLASH_Increament_Flash_Addr();
//			if (Write_Address.Page == INIT_WRITE_PAGE) {
//				Block_Erase(Flash_Address);
//				HAL_Delay(10);
//			}
//
//			Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(Write_Address.Page,
//					Write_Address.Block, Write_Address.Plane);
//
//			Flash_write(pBuffer, Temp_Address, Remaining_Bytes);
//		};
//	}
//
//}

/*******************************************************************************
 * Function Name  : SPI_FLASH_BufferWrite
 * Description    : To load the Program in the Flash Memory So that it will not vanish when power off.
 * Input          : - pBuffer : pointer to the buffer  containing the data to be
 *                    written to the FLASH.
 *                  - Write_Addr : FLASH's internal address to write to.
 *                  - Total_Bytes_To_Write : number of bytes to write to the FLASH.
 * Output         : None
 * Return         : void
 *******************************************************************************/
//void Bulk_Read(char *pBuffer, unsigned long Write_Addr,
//		unsigned short Total_Bytes_To_Write) {
//	unsigned char NumOfPage = 0;
//	unsigned short Remaining_Bytes = 0;
//	unsigned int buffer_Index = 0;
//
//	Temp_Address = Write_Addr;
//	NumOfPage = Total_Bytes_To_Write / SPI_FLASH_PageSize;
//
//	if (NumOfPage == 0) /* Total_Bytes_To_Write < SPI_FLASH_PageSize */
//	{
//		Read_Page(pBuffer, Temp_Address, Total_Bytes_To_Write);
//	} else /* Total_Bytes_To_Write > SPI_FLASH_PageSize */
//	{
//		while (NumOfPage--) {
//			memset(Local_Buffer, 0, sizeof(Local_Buffer));
//			Read_Page(Local_Buffer, Temp_Address, SPI_FLASH_PageSize);
//
//			for (unsigned int i = 0; i < SPI_FLASH_PageSize; i++) {
//				pBuffer[buffer_Index++] = Local_Buffer[i];
//			}
//
//			SPI_FLASH_Increament_Flash_Addr();
//			Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(Write_Address.Page,
//					Write_Address.Block, Write_Address.Plane);
//
//			Remaining_Bytes = Total_Bytes_To_Write = Total_Bytes_To_Write
//					- SPI_FLASH_PageSize;
//			if (Remaining_Bytes >= SPI_FLASH_PageSize)
//				continue;
//			else
//				break;
//		}
//		if (Remaining_Bytes != 0) {
//			SPI_FLASH_Increament_Flash_Addr();
//
//			Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(Write_Address.Page,
//					Write_Address.Block, Write_Address.Plane);
//
//			memset(Local_Buffer, 0, sizeof(Local_Buffer));
//			Read_Page(Local_Buffer, Temp_Address, Remaining_Bytes);
//			for (unsigned int i = 0; i < SPI_FLASH_PageSize; i++) {
//				pBuffer[buffer_Index++] = Local_Buffer[i];
//			}
//		}
//	}
//}
