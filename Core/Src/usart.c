/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;
DMA_NodeTypeDef Node_GPDMA1_Channel4;
DMA_QListTypeDef List_GPDMA1_Channel4;
DMA_HandleTypeDef handle_GPDMA1_Channel4;

/* UART5 init function */
void MX_UART5_Init(void)
{

	/* USER CODE BEGIN UART5_Init 0 */

	/* USER CODE END UART5_Init 0 */

	/* USER CODE BEGIN UART5_Init 1 */

	/* USER CODE END UART5_Init 1 */
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART5_Init 2 */

	/* USER CODE END UART5_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

	/* USER CODE BEGIN USART6_Init 0 */

	/* USER CODE END USART6_Init 0 */

	/* USER CODE BEGIN USART6_Init 1 */

	/* USER CODE END USART6_Init 1 */
	huart6.Instance = USART6;
	huart6.Init.BaudRate = 115200;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart6, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart6, UART_RXFIFO_THRESHOLD_8_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(&huart6) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART6_Init 2 */

	/* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	DMA_NodeConfTypeDef NodeConfig= {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if(uartHandle->Instance==UART5)
	{
		/* USER CODE BEGIN UART5_MspInit 0 */

		/* USER CODE END UART5_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
		PeriphClkInitStruct.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler();
		}

		/* UART5 clock enable */
		__HAL_RCC_UART5_CLK_ENABLE();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**UART5 GPIO Configuration
    PB5     ------> UART5_RX
    PB6     ------> UART5_TX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF14_UART5;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* UART5 DMA Init */
		/* GPDMA1_REQUEST_UART5_RX Init */
		NodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
		NodeConfig.Init.Request = GPDMA1_REQUEST_UART5_RX;
		NodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
		NodeConfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
		NodeConfig.Init.SrcInc = DMA_SINC_FIXED;
		NodeConfig.Init.DestInc = DMA_DINC_INCREMENTED;
		NodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
		NodeConfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
		NodeConfig.Init.SrcBurstLength = 1;
		NodeConfig.Init.DestBurstLength = 1;
		NodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
		NodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		NodeConfig.Init.Mode = DMA_NORMAL;
		NodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
		NodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
		NodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
		if (HAL_DMAEx_List_BuildNode(&NodeConfig, &Node_GPDMA1_Channel4) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_DMAEx_List_InsertNode(&List_GPDMA1_Channel4, NULL, &Node_GPDMA1_Channel4) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_DMAEx_List_SetCircularMode(&List_GPDMA1_Channel4) != HAL_OK)
		{
			Error_Handler();
		}

		handle_GPDMA1_Channel4.Instance = GPDMA1_Channel4;
		handle_GPDMA1_Channel4.InitLinkedList.Priority = DMA_HIGH_PRIORITY;
		handle_GPDMA1_Channel4.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
		handle_GPDMA1_Channel4.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
		handle_GPDMA1_Channel4.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		handle_GPDMA1_Channel4.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
		if (HAL_DMAEx_List_Init(&handle_GPDMA1_Channel4) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel4, &List_GPDMA1_Channel4) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmarx, handle_GPDMA1_Channel4);

		if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel4, DMA_CHANNEL_NPRIV) != HAL_OK)
		{
			Error_Handler();
		}

		/* UART5 interrupt Init */
		HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(UART5_IRQn);
		/* USER CODE BEGIN UART5_MspInit 1 */

		/* USER CODE END UART5_MspInit 1 */
	}
	else if(uartHandle->Instance==USART1)
	{
		/* USER CODE BEGIN USART1_MspInit 0 */

		/* USER CODE END USART1_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
		PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler();
		}

		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		/* USER CODE BEGIN USART1_MspInit 1 */

		/* USER CODE END USART1_MspInit 1 */
	}
	else if(uartHandle->Instance==USART6)
	{
		/* USER CODE BEGIN USART6_MspInit 0 */

		/* USER CODE END USART6_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6;
		PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler();
		}

		/* USART6 clock enable */
		__HAL_RCC_USART6_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* USART6 interrupt Init */
		HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART6_IRQn);
		/* USER CODE BEGIN USART6_MspInit 1 */

		/* USER CODE END USART6_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

	if(uartHandle->Instance==UART5)
	{
		/* USER CODE BEGIN UART5_MspDeInit 0 */

		/* USER CODE END UART5_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_UART5_CLK_DISABLE();

		/**UART5 GPIO Configuration
    PB5     ------> UART5_RX
    PB6     ------> UART5_TX
		 */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_6);

		/* UART5 DMA DeInit */
		HAL_DMA_DeInit(uartHandle->hdmarx);

		/* UART5 interrupt Deinit */
		HAL_NVIC_DisableIRQ(UART5_IRQn);
		/* USER CODE BEGIN UART5_MspDeInit 1 */

		/* USER CODE END UART5_MspDeInit 1 */
	}
	else if(uartHandle->Instance==USART1)
	{
		/* USER CODE BEGIN USART1_MspDeInit 0 */

		/* USER CODE END USART1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

		/* USART1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		/* USER CODE BEGIN USART1_MspDeInit 1 */

		/* USER CODE END USART1_MspDeInit 1 */
	}
	else if(uartHandle->Instance==USART6)
	{
		/* USER CODE BEGIN USART6_MspDeInit 0 */

		/* USER CODE END USART6_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART6_CLK_DISABLE();

		/**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
		 */
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

		/* USART6 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART6_IRQn);
		/* USER CODE BEGIN USART6_MspDeInit 1 */

		/* USER CODE END USART6_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
