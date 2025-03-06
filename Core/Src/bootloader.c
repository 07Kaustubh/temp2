#include "bootloader.h"

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim6;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern RTC_HandleTypeDef hrtc;
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern CRYP_HandleTypeDef hcryp;

typedef void (*pFunction)(void);

unsigned long CP_Address;

uint32_t L1 = 0x08000000;	   // Sector 0-15  or 0-31
uint32_t L2 = 0x08040000;	 // Sector 16-31 or 32 to 64
uint32_t L3 = 0x08080000;	  // Sector 32-48   or 64 to 96
extern uint32_t VT_OFF;
pFunction Jump_To_Application;
uint64_t CP;
uint64_t CD;
extern uint64_t SP;
extern char PHnum[10];
void Application_jump() {
	if ( CD == 1) {
				if(!CopyFlashSectors(L3, L2, 0x40000)) {
				CD = 1;
				CP = 0;
				}
				else
				{
				CD = 0;
				CP = 1;
				SP =1;
				writeF_CP((uint64_t) CP, (uint64_t) CD, (uint64_t) SP);
				HAL_NVIC_SystemReset();
				}
			}

	if (CP == 1) {

			void (*app_reset_handler)(
				void) = (void (*)(void))(*((volatile uint32_t*) (L2 + 4U)));

		// Reset the SysTick
		SysTick->CTRL = 0;
		SysTick->LOAD = 0;
		SysTick->VAL = 0;

		// Set the main stack pointer to the application's stack pointer
		__set_MSP(*(volatile uint32_t*) L2);
		VT_OFF = 0x40000;

		SCB->VTOR = L2;
		// Jump to the application's reset handler
		app_reset_handler();
	} else {
		VT_OFF = 0x00000;
		SCB->VTOR = L1;
		writeF_CP((uint64_t) 0, (uint64_t) CD, (uint64_t) 0);
	}
}

void setupBootLoader() {
	char CP_Data[8] = { 0 };
	char CD_Data[8] = { 0 };
//	char Sp_Data[8] = { 0 };
	FLASH_Read_Buff(Current_App_LOC, CP_Data, 8);
	FLASH_Read_Buff(Current_App_LOC + 16, CD_Data, 8);
//	FLASH_Read_Buff(Current_App_LOC + 32, Sp_Data,8);
	CP = CP_Data[0];
	CD = CD_Data[0];
//	SP = Sp_Data[0];
	Application_jump();
}


