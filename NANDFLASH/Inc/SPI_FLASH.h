/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __SPI_FLASH_2GB_H
#define  __SPI_FLASH_2GB_H


#include "main.h"


#define CONFIG_ADD                                      0x0000
#define DEFAULT_ADD                                     0x1000
#define TEST_ADDRESS                                    0x22000
#define SPI_FLASH_START_ADDRESS                         0x1100

#define SPI_FLASH_PageSize                      0x7FF //2048
#define SPI_FLASH_Block_SIZE                    0x1FFFF
#define SPI_FLASH_SPARE_AREA                    0x80 //128
#define DATA_SIZE_IN_BYTES                      0x7F8 //0x7E8 //2024
#define PAGE_OFFSET                             0x18

#define FLASH_UNIQUE_ID                         0x2C24 /* FLASH UNIQUE ID */
#define READ_ID_CMD                             0x9F  /* Read Status Register instruction RDID */
#define DUMMY_BYTE                              0x00  /* Send Dummy Byte to provide the Required Clock*/

#define WRITE_TO_MEMORY_CMD                     0x02  /* Write to Memory instruction (WRITE) */
#define WRITE_STATUS_REG                        0x01  /* Write Status Register instruction (WRSR)*/
#define WRITE_EN_CMD                            0x06  /* Write enable instruction WREN */
#define FLASH_RESET								0xFF // RESET_Flash
#define READ_CMD                                0x03  /* Read from Memory instruction READ */
#define READ_STATUS_REG                         0x05  /* Read Status Register instruction RDSR*/
#define BLOCK_LOCK_REGISTER                     0xA0  /* Read Block Lock Register */
#define CONFIG_REGISTER                         0xB0  /* Read Config Register */
#define WRITE_DIS_CMD                           0x04  /* Write disable instruction */
#define BLOCK_ERASE                             0xD8  /* Block Erase instruction D8 - is for 64k block, 52 is for 32k block*/ 
#define CHIP_ERASE                              0xC7  /* Bulk Erase instruction (CE)*/

#define GET_FEATURES                            0x0F /* GET FEATURES command is to monitor the Status of the process*/
#define SET_FEATURES                            0x1F /* Set this CMD to Unblock all the Blocks*/
#define STATUS_REG_ADDR                         0xC0 /* status register can be read by issuing the GET_FEATURE CMD, followed by the feature address*/
#define PAGE_READ_CMD                           0X13 /*page read CMD to cache*/
#define READ_PAGE_CACHE_RANDOM 			0x30  /* Cache read Random*/
#define READ_PAGE_CACHE_LAST 			0x3F  /* read Ending of cache */
#define READ_FROM_CACHE 			0x03  /* Read from the Cache Register over 1 I/O line */
#define PROGRAM_EXECUTE		                0x10  /* 3 0 0 Array program */
#define PROGRAM_LOAD 	            	        0x02  /* Load program data into cache register on SI */
#define PERMANENT_BLOCK_LOCK_PROTECTION         0x2C  /* 3 0 0 Permanently protect a specific group of blocks */

/**Read from the Cache Register over 2,4 I/O line - We can not utilize this functinality bcz we have only 1 MISO line**/

#define READ_FROM_CACHE_x2 			0x3B  /* SI input will be turn to SO to Tx the data over 2 I/O line*/
#define READ_FROM_CACHE_x4 			0x6B  /* SI/ WP/ HOLD PIN will be turn input to Tx the data over 4 I/O line */
#define READ_FROM_CACHE_Dual_IO 		0xBB  /* Similar to the READ_FROM_CACHE_x2 */
#define READ_FROM_CACHE_Quad_IO   		0xEB  /* Similar to the READ_FROM_CACHE_Dual_IO */

#define PROGRAM LOAD_x4 		        0x32  /* 2 0 1 to 2176 Load program data into cache register on SO[3:0] */
#define PROGRAM_LOAD_RANDOM_DATA  	        0x84  /* To load new data into the cache register. This data will replace or update the existing data within the page. */
#define PROGRAM_LOAD_RANDOM_DATA_x4 	        0x34  /* 2 0 1 to 2176 Overwrite cache register with input data on SO[3:0] */
/**-------------------------------------------------------------------------------------------------------------------**/

#define INIT_WRITE_PAGE	        		0 /* initial writing Page */
#define INIT_WRITE_BLOCK   		        0 /* initial writing Block */
#define INIT_WRITE_PLANE                        0 /* initial writing plane */

#define UPDATE_ADDR_PAGE      		        0 /* Addrress Update Page */
#define UPDATE_ADDR_BLOCK   		        0 /* Addrress Update Block */
#define UPDATE_PLANE_ADDR                       0

#define WRITE_PAGES_LIMIT   		        64 /* Page limit of writing block */
#define PAGES_TO_WRITE   			1 /* Pages to be written at once */
#define PAGES_TO_READ   			1 /* Pages to be Read(pronounce: /rEEd/) at once */
#define LAST_BLOCK_ADDR   			1023 /* MAXIMUM BLOCK IN 1 PLANE */
#define MAX_PLANE_AVAILABLE                     1 //0-1    /* MAXIMUM PLANE IN 2GB DEVICE */

#define ALL_GARBAGE_VALUE   		        255 /* 2GB Nand '�' Garbage */

#define TOTAL_BYTE_SIZE                         1024 /* 2GB Nand Byte Stream Size */
#define BUFFER_SIZE                             2048 /* 2GB Nand Main Buffer inside Ram -> Size */
#define MAIN_BUFF_FLUSH_LIMIT                   2048 /* 2GB Nand Main Buffer limit before Flush */

#define SET_ECC_MODE                            0x10
#define RESET_ECC_MODE                          0x00

#define SPI3_CS_Pin                     LL_GPIO_PIN_15
#define SPI3_CS_GPIO_Port               GPIOA
#define FLASH_ENABLE                    SPI3_CS_GPIO_Port->BRR  = SPI3_CS_Pin
#define FLASH_DISABLE                   SPI3_CS_GPIO_Port->BSRR = SPI3_CS_Pin

#define TIMEOUT_MS 200


typedef struct
{
  uint8_t Page;    /* 2GB NAND memory Page address  */

  uint16_t Block;   /* 2GB NAND memory Block address */

  uint8_t Plane;   /* 2GB NAND memory Zone address  */

  unsigned long Address;

}FLASH_AddressTypeDef;




unsigned char SPI_FLASH_SendByte(unsigned char byte);
unsigned long SPI_FLASH_Creat_ADDR_TO_WRITE(unsigned int Page_Addr,
		unsigned int Block_Addr, unsigned int Plane_Addr);
void Get_SPI_FLASH_Config(void);
void Write_SPI_FLASH_Config(void);
void Block_Lock_Protection_Disable(void);
void SPI_FLASH_Check_ECC_Mode(void);
void SPI_FLASH_Read_Block_Lock_Register(void);
void SPI_FLASH_Reset_Write_ADD(void);
void SPI_FLASH_Increament_Flash_Addr(void);
void Parameter_Page(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WriteDisable(void);
void Unlock_ALL_Blocks(void);
void SPI_FLASH_ReadID(void);
void SPI_FLASH_Wait_For_READ_WRITE_END(void);
void  Block_Erase(unsigned long SectorAddr);
uint8_t Read_Page(char *pBuffer, unsigned long Read_Addr,
		unsigned short Total_Bytes_To_Read);
uint8_t Flash_write(char *pBuffer, unsigned long Write_Addr,
		unsigned short Total_Bytes_To_Write);
void SPI_FLASH_Full_Erase(void);
uint8_t read_status_register();
void Write_Parameters_In_Flash(void);
void SPI_FLASH_Write_ECC_Bit(uint8_t ECC_Mode);
unsigned int SPI_FLASH_ADDR_TO_WRITE(unsigned int Page_Addr, unsigned int Block_Addr);
void Bulk_Write(char* pBuffer, unsigned long Write_Addr, unsigned short Total_Bytes_To_Write);
void Bulk_Read(char* pBuffer, unsigned long Write_Addr, unsigned short Total_Bytes_To_Write);





















#endif /* __SPI_FLASH_2GB_H */
