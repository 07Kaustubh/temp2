#include"FLASH_MEM.h"

#include "stack.h"

/**********************************************Nripender Code************************************************/
FLASH_AddressTypeDef write_Address, L86_Read_Address;

int L86DataCount = 5;
uint64_t NPage = 0, NBlock = 0;
extern Stack dataQueue;
extern Stack packetHistoryQueue;
extern bool NF_WR_FLAG;
extern bool NF_RD_FLAG, FLAG_TCP_Fail, FLAG_TCP_Pass;
extern IWDG_HandleTypeDef hiwdg;
//	FLASH_AddressTypeDef address;
//	Assign values to the address components
//	address.Page = 0;// size of one page is 2048
//	address.Block = 0;// size of block 64 page
//	address.Plane = 0;// size of 1024 block and we have 2 plane
//	unsigned long Temp_Address = SPI_FLASH_Creat_ADDR_TO_WRITE(address.Page, address.Block, address.Plane);
unsigned long caculateAddress(int Page, int Block, int Plane) {
	return SPI_FLASH_Creat_ADDR_TO_WRITE(Page, Block, Plane);
}

bool ReadFromNANDFLASH(uint8_t *readBuffer, unsigned long address,
		unsigned short size) {
	Unlock_ALL_Blocks();

	Block_Lock_Protection_Disable();
	if (!Read_Page(readBuffer, address, size))
		return false;
	return true;
}
bool WriteInToNANDFLASH(uint8_t *pBuffer, unsigned long address,
		unsigned short size) {
	return Flash_write(pBuffer, address, size);
}
bool writeL86Data(uint8_t *dataBuffer, int size) {
	bool status = false;
	if (size + 1 > SIZE_OF_PAGE) {
		printc(MinDelay, "Page can't be write ");

		return false;
	}
	write_Address.Block = NBlock;
	write_Address.Page = NPage;
	if (write_Address.Page == 0) {
		Unlock_ALL_Blocks();
		Block_Lock_Protection_Disable();
		Block_Erase(caculateAddress(write_Address.Page, write_Address.Block, 0));
	}
	Unlock_ALL_Blocks();
	Block_Lock_Protection_Disable();
	printc(MinDelay,"\nlength of wrt data %d\n",strlen(dataBuffer));
	status = WriteInToNANDFLASH(dataBuffer,
			caculateAddress(write_Address.Page, write_Address.Block, 0), size);
	printc(MinDelay, "\n Write Address \n%d", write_Address.Page);
	(write_Address.Page)++;
	if (write_Address.Page == WRITE_PAGES_LIMIT) {
		write_Address.Page = INIT_WRITE_PAGE; /* RESET THE PAGE NO.*/
		(write_Address.Block)++; /* INCREAMENT THE BLOCK NO.*/
	}
	NPage = (uint64_t) write_Address.Page;
	NBlock = (uint64_t) write_Address.Block;
	return status;
}

/*
 * Following function(deleteL86Data) will remove data from starting as we trying to implement FIFO methods *?
 */

/*
 * Following function resetL86Data will use to remove all data and start for block1
 */
void resetL86Data() {
	write_Address.Page = INIT_WRITE_PAGE;
	write_Address.Block = INIT_WRITE_BLOCK;
	write_Address.Plane = INIT_WRITE_PLANE;
	L86_Read_Address.Page = INIT_WRITE_PAGE;
	L86_Read_Address.Block = INIT_WRITE_BLOCK;
	L86_Read_Address.Plane = INIT_WRITE_PLANE;
}
// this function will return the value of pa
uint8_t His_rd_buff[2048]={0};
uint8_t Rbuf[2048] = {0};
bool RdN_Block() {

	if (NBlock != 0 && NPage == 0) {
		NBlock--;
		NPage = 63;
	}
	while (NPage != 0) {
		NF_RD_FLAG = true;
		NPage--;
		clearBuffer(His_rd_buff, sizeof(His_rd_buff));
		ReadFromNANDFLASH(His_rd_buff, caculateAddress(NPage, NBlock, 0), 2048);
		clearBuffer(Rbuf, sizeof(Rbuf));
		strncpy((char*)Rbuf, (char*)&His_rd_buff[1], sizeof(His_rd_buff));
		printc(100,"size of ReadData : %d,NBlock :%i,NPage :%i\n",strlen(Rbuf),NBlock,NPage);
		if(strlen(Rbuf)<2000){
			push(&packetHistoryQueue, Rbuf, strlen((char*)Rbuf));
		}
		else{
			printc(100,"size is bigger\n");
		}
		HAL_IWDG_Refresh(&hiwdg);
	}

	if (NPage != 0)
		return false;

	NF_WR_FLAG = false;
	NF_RD_FLAG = false;
	FLAG_TCP_Fail = false;
	FLAG_TCP_Pass = false;

	return true;
}


/*
 * Following function readL86Data will read the First value of FIFO Queue
 */

bool readL86Data(uint8_t *L86BufferData, int size) {
	if ((write_Address.Block <= L86_Read_Address.Block)
			&& (write_Address.Page <= L86_Read_Address.Page)) {
		return false;
	}
	ReadFromNANDFLASH(L86BufferData,
			caculateAddress(L86_Read_Address.Page, L86_Read_Address.Block,
					L86_Read_Address.Plane), size);
}

/*
 *Following function will used to setUp for Flash
 *
 * */

