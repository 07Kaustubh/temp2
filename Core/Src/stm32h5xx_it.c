/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h5xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32h5xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern int FLAG_IGN ;
extern int   sleepcount;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim6;
extern DMA_NodeTypeDef Node_GPDMA1_Channel4;
extern DMA_QListTypeDef List_GPDMA1_Channel4;
extern DMA_HandleTypeDef handle_GPDMA1_Channel4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
	uint32_t hfsr = SCB->HFSR;
	uint32_t mmfsr = SCB->CFSR & 0xFF;       // Memory Management Fault Status
	uint32_t bfsr = (SCB->CFSR >> 8) & 0xFF; // Bus Fault Status
	uint32_t ufsr = (SCB->CFSR >> 16) & 0xFF; // Usage Fault Status

	 char buffer[256] = {0};
	    int len = snprintf(buffer, sizeof(buffer),
	                       "NMI detected!\nHFSR: 0x%08lX\nMMFSR: 0x%02lX\nBFSR: 0x%02lX\nUFSR: 0x%02lX\n\n\r",
	                       hfsr, mmfsr, bfsr, ufsr);
	    printc(MinDelay,buffer);

	    HAL_NVIC_SystemReset();
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void){
  /* USER CODE BEGIN HardFault_IRQn 0 */
	uint32_t hfsr = SCB->HFSR;
	uint32_t mmfsr = SCB->CFSR & 0xFF;       // Memory Management Fault Status
	uint32_t bfsr = (SCB->CFSR >> 8) & 0xFF; // Bus Fault Status
	uint32_t ufsr = (SCB->CFSR >> 16) & 0xFF; // Usage Fault Status

	 char buffer[256] = {0};
	    int len = snprintf(buffer, sizeof(buffer),
	                       "HardFault detected!\nHFSR: 0x%08lX\nMMFSR: 0x%02lX\nBFSR: 0x%02lX\nUFSR: 0x%02lX\n\n\r",
	                       hfsr, mmfsr, bfsr, ufsr);
	    printc(MinDelay,buffer);

	    HAL_NVIC_SystemReset();
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
	uint32_t mmfsr = SCB->CFSR & 0xFF; // Memory Management Fault Status

	    char buffer[256] = {0};
	    snprintf(buffer, sizeof(buffer),
	             "Memory Management Fault detected!\nMMFSR: 0x%02lX\n\n\r",
	             mmfsr);
	    printc(MinDelay, buffer);
	HAL_NVIC_SystemReset();
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
	 uint32_t bfsr = (SCB->CFSR >> 8) & 0xFF; // Bus Fault Status

	    char buffer[256] = {0};
	    snprintf(buffer, sizeof(buffer),
	             "Bus Fault detected!\nBFSR: 0x%02lX\n\n\r",
	             bfsr);
	    printc(MinDelay, buffer);
	HAL_NVIC_SystemReset();
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

    uint32_t ufsr = (SCB->CFSR >> 16) & 0xFF; // Usage Fault Status

    char buffer[256] = {0};
    snprintf(buffer, sizeof(buffer),
             "Usage Fault detected!\nUFSR: 0x%02lX\n\n\r",
             ufsr);
    printc(MinDelay, buffer);
	HAL_NVIC_SystemReset();
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RTC non-secure interrupt.
  */
void RTC_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_IRQn 0 */

  /* USER CODE END RTC_IRQn 0 */
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_IRQn 1 */
	HAL_PWR_DisableSleepOnExit();
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
  /* USER CODE END RTC_IRQn 1 */
}

/**
  * @brief This function handles EXTI Line8 interrupt.
  */
void EXTI8_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI8_IRQn 0 */

  /* USER CODE END EXTI8_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_8) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_8);
    /* USER CODE BEGIN LL_EXTI_LINE_8_FALLING */
    HAL_PWR_DisableSleepOnExit();
    onMessage();
    /* USER CODE END LL_EXTI_LINE_8_FALLING */
  }
  /* USER CODE BEGIN EXTI8_IRQn 1 */

  /* USER CODE END EXTI8_IRQn 1 */
}

/**
  * @brief This function handles EXTI Line15 interrupt.
  */
void EXTI15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_IRQn 0 */

  /* USER CODE END EXTI15_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_15) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_15);
    /* USER CODE BEGIN LL_EXTI_LINE_15_FALLING */

    /* USER CODE END LL_EXTI_LINE_15_FALLING */
  }
  /* USER CODE BEGIN EXTI15_IRQn 1 */
  onTemperCallback();
  /* USER CODE END EXTI15_IRQn 1 */
}

/**
  * @brief This function handles GPDMA1 Channel 4 global interrupt.
  */
void GPDMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN GPDMA1_Channel4_IRQn 0 */

  /* USER CODE END GPDMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel4);
  /* USER CODE BEGIN GPDMA1_Channel4_IRQn 1 */

  /* USER CODE END GPDMA1_Channel4_IRQn 1 */
}

/**
  * @brief This function handles FDCAN1 interrupt 0.
  */
void FDCAN1_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 0 */
	sleepcount =0;
  /* USER CODE END FDCAN1_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan1);
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 1 */
	HAL_PWR_DisableSleepOnExit();
  /* USER CODE END FDCAN1_IT0_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  enableIdle();

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */

  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */

  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */
  openPrintUartIdle();
  /* USER CODE END USART6_IRQn 1 */
}

/**
  * @brief This function handles FDCAN2 interrupt 0.
  */
void FDCAN2_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 0 */
//	sleepcount =0;
  /* USER CODE END FDCAN2_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan2);
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 1 */
//	HAL_PWR_DisableSleepOnExit();
  /* USER CODE END FDCAN2_IT0_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void EXTI0_IRQHandler(void) {
	printc(MinDelay, "WokeBY 1");
	if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_0) != RESET) {
		// Clear the EXTI line 0 interrupt flag
		LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_0);

		FLAG_IGN =1;
		// Disable sleep on exit
		HAL_PWR_DisableSleepOnExit();

		// Disable EXTI line 0 interrupt to avoid re-entering the interrupt handler
		HAL_NVIC_DisableIRQ(EXTI0_IRQn);

		printc(MinDelay, "Wakeby EXTI");

		// Perform wakeup routine
//        WAKEUP();
	}
}
/* USER CODE END 1 */
