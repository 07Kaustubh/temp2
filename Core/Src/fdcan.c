/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    fdcan.c
 * @brief   This file provides code for the configuration
 *          of the FDCAN instances.
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
#include "fdcan.h"
FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;
/* USER CODE BEGIN 0 */
RxCanConfiguration CanConfigurationTool;
//TxCanConfiguration TxCanConfigurationTool;
uint8_t StandardIdCount = 0,ExtendedIDCount = 0,canCount = 8;
uint32_t ConfigIdTypeTx,ConfigFilterTx;
uint8_t TxData[3][8]={{0}};
#define MAX_BUFFER_SIZE 32
#define PADDING_CHAR 0x00

bool can_config() {
	ConfigIdTypeTx=FDCAN_STANDARD_ID;
	ConfigFilterTx=0x123;
	uint8_t *str = getConfigValue(Config_CanNbr);
	uint8_t splitParts[5][100]={{0}};
	// Split the string using `Ù` as delimiter
	if (!splitHexString(str, '\xd9', splitParts, 2)) {
		printc(MinDelay, "Invalid CAN configuration format\n");
		return false;
	}
	ExtendedIDCount = strtol(splitParts[0], NULL, 10);
	StandardIdCount = strtol(splitParts[1], NULL, 10);
	if (StandardIdCount == 0 && ExtendedIDCount == 0)
		return false;
	canCount = (StandardIdCount > ExtendedIDCount) ? StandardIdCount : ExtendedIDCount;
	for (int i = 0; i < canCount; i++) {
		uint8_t *canStr = getConfigValue(Config_CanF0 + i);
		memset(splitParts, 0, sizeof(splitParts));
		// Split the string using the delimiter '\xD9' (Ù)
		if (!splitHexString(canStr,'\xd9', splitParts, 8)) {
			printc(MinDelay, "Wrong data format in CONFIG table\n");
			return false;
		}
		CanConfigurationTool.ConfigIdType[i] = (strtol(splitParts[0], NULL, 10) == 1) ? FDCAN_STANDARD_ID : FDCAN_EXTENDED_ID;
		CanConfigurationTool.configFilterType[i] = strtol(splitParts[1], NULL, 10);
		CanConfigurationTool.ConfigFilter1[i] = strtoul(splitParts[2], NULL, 16);
		CanConfigurationTool.ConfigFilter2[i] = strtoul(splitParts[3], NULL, 16);
	}
	memset(splitParts, 0, sizeof(splitParts));
	for (int i = 0; i <3; i++) {
		uint8_t *TxStr= getConfigValue(Config_CAN_TX1+i);
		if (!splitHexString(TxStr,'\xd9',splitParts, 3)) {
			printc(MinDelay,"Wrong data format in CONFIG tableTx\n");
			//			return false;
		}

		if(getConfigValue(Config_CAN_TX1)!=NULL){
			ConfigFilterTx =(strtol(splitParts[0],  NULL, 10) == 1) ? FDCAN_STANDARD_ID : FDCAN_EXTENDED_ID;
			ConfigIdTypeTx = strtoul(splitParts[1], NULL, 16);
			string_to_hex_array(splitParts[2], TxData[i]);
		}
	}
	return true;  // Return true if all configurations are valid
}

FDCAN_RxHeaderTypeDef RxHeader, RxHeader1;
FDCAN_TxHeaderTypeDef TxHeader, TxHeader1;
/* USER CODE END 0 */
/* FDCAN2 init function */
void MX_FDCAN2_Init(void) {
	/* USER CODE BEGIN FDCAN2_Init 0 */
	CAN_ON();
	/* USER CODE END FDCAN2_Init 0 */

	/* USER CODE BEGIN FDCAN2_Init 1 */
	if (!can_config())
		return;
	/* USER CODE END FDCAN2_Init 1 */
	hfdcan2.Instance = FDCAN2;
	hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV1;
	hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan2.Init.AutoRetransmission = ENABLE;
	hfdcan2.Init.TransmitPause = DISABLE;
	hfdcan2.Init.ProtocolException = ENABLE;
	hfdcan2.Init.NominalPrescaler = 1;
	hfdcan2.Init.NominalSyncJumpWidth = 13;
	hfdcan2.Init.NominalTimeSeg1 = 86;
	hfdcan2.Init.NominalTimeSeg2 = 13;
	hfdcan2.Init.DataPrescaler = 4;
	hfdcan2.Init.DataSyncJumpWidth = 9;
	hfdcan2.Init.DataTimeSeg1 = 15;
	hfdcan2.Init.DataTimeSeg2 = 9;
	hfdcan2.Init.StdFiltersNbr = StandardIdCount;
	hfdcan2.Init.ExtFiltersNbr = ExtendedIDCount;
	hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

	if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN FDCAN2_Init 2 */
	FDCAN_FilterTypeDef sFilter2Config = {0};              /* clearing all the filter configuration */             /* clearing all the filter configuration */

	TxHeader1.Identifier          = ConfigIdTypeTx;// 0x63C;                /*  taking default identifier  */
	TxHeader1.IdType              = ConfigFilterTx;//FDCAN_STANDARD_ID;    /*  0 and 0x7FF, if IdType is FDCAN_STANDARD_ID , 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID  */
	TxHeader1.TxFrameType         = FDCAN_DATA_FRAME;     /*  Specifies the frame type of the message that will be transmitted.  */
	TxHeader1.DataLength          = FDCAN_DLC_BYTES_8;    /*  the length of the transmitted frame 8 bytes.  */
	TxHeader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;     /*  FDCAN_error_state_indicator,node is error active  */
	TxHeader1.BitRateSwitch       = FDCAN_BRS_OFF;        /*  frames transmitted/received without bit rate switching  */
	TxHeader1.FDFormat            = FDCAN_CLASSIC_CAN;    /*  Frame transmitted/received in Classic CAN format  */
	TxHeader1.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;   /*  Do not store Tx events */
	TxHeader1.MessageMarker       = 0;
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT,
			FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
	for(int i=0;i<canCount;i++){
		sFilter2Config.IdType = CanConfigurationTool.ConfigIdType[i];

		sFilter2Config.FilterIndex = i;

		sFilter2Config.FilterType = CanConfigurationTool.configFilterType[i];

		sFilter2Config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

		sFilter2Config.FilterID1 = CanConfigurationTool.ConfigFilter1[i];

		sFilter2Config.FilterID2 = CanConfigurationTool.ConfigFilter2[i]; /* For acceptance, MessageID and FilterID1 must match exactly */
		/* Configure the FDCAN reception filter */
		if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilter2Config) != HAL_OK) {
			Error_Handler();
		}
	}
	/* Start the FDCAN module */
	if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
		Error_Handler();
	}
	/* Enable interrupts */
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
			0) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_FIFO_EMPTY, 0)
			!= HAL_OK) {
		/* Notification Error */
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_COMPLETE, FDCAN_TX_BUFFER0) != HAL_OK)
	{
		//	   Routine_takeErr_logs();
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_COMPLETE, FDCAN_TX_BUFFER1) != HAL_OK)
	{
		//	   Routine_takeErr_logs();
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_COMPLETE,FDCAN_TX_BUFFER2) != HAL_OK)
	{
		//	   Routine_takeErr_logs();
		Error_Handler();
	}
	if(HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_LIST_TX_FIFO_ERROR,0) != HAL_OK)
	{
		//	   Routine_takeErr_logs();
		Error_Handler();
	}

	HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);

	/* USER CODE END FDCAN2_Init 2 */

}

static uint32_t HAL_RCC_FDCAN_CLK_ENABLED = 0;

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *fdcanHandle) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (fdcanHandle->Instance == FDCAN1) {
		/* USER CODE BEGIN FDCAN1_MspInit 0 */

		/* USER CODE END FDCAN1_MspInit 0 */
		/* FDCAN1 clock enable */
		HAL_RCC_FDCAN_CLK_ENABLED++;
		if (HAL_RCC_FDCAN_CLK_ENABLED == 1) {
			__HAL_RCC_FDCAN_CLK_ENABLE();
		}

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**FDCAN1 GPIO Configuration
		 PA11     ------> FDCAN1_RX
		 PA12     ------> FDCAN1_TX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* FDCAN1 interrupt Init */
		HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
		/* USER CODE BEGIN FDCAN1_MspInit 1 */

		/* USER CODE END FDCAN1_MspInit 1 */
	} else if (fdcanHandle->Instance == FDCAN2) {
		/* USER CODE BEGIN FDCAN2_MspInit 0 */

		/* USER CODE END FDCAN2_MspInit 0 */
		/* FDCAN2 clock enable */
		HAL_RCC_FDCAN_CLK_ENABLED++;
		if (HAL_RCC_FDCAN_CLK_ENABLED == 1) {
			__HAL_RCC_FDCAN_CLK_ENABLE();
		}

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**FDCAN2 GPIO Configuration
		 PB12     ------> FDCAN2_RX
		 PB13     ------> FDCAN2_TX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* FDCAN2 interrupt Init */
		HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
		/* USER CODE BEGIN FDCAN2_MspInit 1 */

		/* USER CODE END FDCAN2_MspInit 1 */
	}
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *fdcanHandle) {

	if (fdcanHandle->Instance == FDCAN1) {
		/* USER CODE BEGIN FDCAN1_MspDeInit 0 */

		/* USER CODE END FDCAN1_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_FDCAN_CLK_ENABLED--;
		if (HAL_RCC_FDCAN_CLK_ENABLED == 0) {
			__HAL_RCC_FDCAN_CLK_DISABLE();
		}

		/**FDCAN1 GPIO Configuration
		 PA11     ------> FDCAN1_RX
		 PA12     ------> FDCAN1_TX
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

		/* FDCAN1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
		/* USER CODE BEGIN FDCAN1_MspDeInit 1 */

		/* USER CODE END FDCAN1_MspDeInit 1 */
	} else if (fdcanHandle->Instance == FDCAN2) {
		/* USER CODE BEGIN FDCAN2_MspDeInit 0 */

		/* USER CODE END FDCAN2_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_FDCAN_CLK_ENABLED--;
		if (HAL_RCC_FDCAN_CLK_ENABLED == 0) {
			__HAL_RCC_FDCAN_CLK_DISABLE();
		}

		/**FDCAN2 GPIO Configuration
		 PB12     ------> FDCAN2_RX
		 PB13     ------> FDCAN2_TX
		 */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12 | GPIO_PIN_13);

		/* FDCAN2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
		/* USER CODE BEGIN FDCAN2_MspDeInit 1 */

		/* USER CODE END FDCAN2_MspDeInit 1 */
	}
}











///* USER CODE BEGIN Header */
///**
// ******************************************************************************
// * @file    fdcan.c
// * @brief   This file provides code for the configuration
// *          of the FDCAN instances.
// ******************************************************************************
// * @attention
// *
// * Copyright (c) 2024 STMicroelectronics.
// * All rights reserved.
// *
// * This software is licensed under terms that can be found in the LICENSE file
// * in the root directory of this software component.
// * If no LICENSE file comes with this software, it is provided AS-IS.
// *
// ******************************************************************************
// */
///* USER CODE END Header */
///* Includes ------------------------------------------------------------------*/
//#include "fdcan.h"
//
///* USER CODE BEGIN 0 */
//
//FDCAN_RxHeaderTypeDef            RxHeader,RxHeader1;
//FDCAN_TxHeaderTypeDef            TxHeader, TxHeader1;
////const uint32_t can_ids[17] = { 0x180FA7F3, 0x1804A7F3, 0x1805A7F3, 0x1806A7F3,
////		0x1807A7F3, 0x1808A7F3, 0x1809A7F3, 0x180AA7F3, 0x180BA7F3, 0x180CA7F3,
////		0x180DA7F3, 0x1824A7F3, 0x18FECA05, 0x18FF0527, 0x18FF0927, 0x18FF5117,
////		0x18FF5417 };
///* USER CODE END 0 */
//
//FDCAN_HandleTypeDef hfdcan1;
//FDCAN_HandleTypeDef hfdcan2;
//
///* FDCAN2 init function */
//void MX_FDCAN2_Init(void){
//	/* USER CODE BEGIN FDCAN2_Init 0 */
//	CAN_ON();
//
//	/* USER CODE END FDCAN2_Init 0 */
//
//	/* USER CODE BEGIN FDCAN2_Init 1 */
//
//	/* USER CODE END FDCAN2_Init 1 */
//	hfdcan2.Instance = FDCAN2;
//	hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV1;
//	hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
//	hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
//	hfdcan2.Init.AutoRetransmission = ENABLE;
//	hfdcan2.Init.TransmitPause = DISABLE;
//	hfdcan2.Init.ProtocolException = ENABLE;
//	hfdcan2.Init.NominalPrescaler = 1;
//	hfdcan2.Init.NominalSyncJumpWidth = 13;
//	hfdcan2.Init.NominalTimeSeg1 = 86;
//	hfdcan2.Init.NominalTimeSeg2 = 13;
//	hfdcan2.Init.DataPrescaler = 4;
//	hfdcan2.Init.DataSyncJumpWidth = 9;
//	hfdcan2.Init.DataTimeSeg1 = 15;
//	hfdcan2.Init.DataTimeSeg2 = 9;
//	hfdcan2.Init.StdFiltersNbr = 4;
//	hfdcan2.Init.ExtFiltersNbr = 0;
//	hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
//	if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/* USER CODE BEGIN FDCAN2_Init 2 */
//	FDCAN_FilterTypeDef sFilter2Config = {0};              /* clearing all the filter configuration */
//
//	/* FDCAN Tx header structure configuration */
//	TxHeader1.Identifier          = 0x63C;                /*  taking default identifier  */
//	TxHeader1.IdType              = FDCAN_STANDARD_ID;    /*  0 and 0x7FF, if IdType is FDCAN_STANDARD_ID , 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID  */
//	TxHeader1.TxFrameType         = FDCAN_DATA_FRAME;     /*  Specifies the frame type of the message that will be transmitted.  */
//	TxHeader1.DataLength          = FDCAN_DLC_BYTES_8;    /*  the length of the transmitted frame 8 bytes.  */
//	TxHeader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;     /*  FDCAN_error_state_indicator,node is error active  */
//	TxHeader1.BitRateSwitch       = FDCAN_BRS_OFF;        /*  frames transmitted/received without bit rate switching  */
//	TxHeader1.FDFormat            = FDCAN_CLASSIC_CAN;    /*  Frame transmitted/received in Classic CAN format  */
//	TxHeader1.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;   /*  Do not store Tx events */
//	TxHeader1.MessageMarker       = 0;
//
//	HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT,
//			FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
//	/* Configure Rx filter */
//	sFilter2Config.IdType = FDCAN_STANDARD_ID;
//
//	sFilter2Config.FilterIndex = 0;
//
//	sFilter2Config.FilterType = FDCAN_FILTER_DUAL;
//
//	sFilter2Config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//
//	sFilter2Config.FilterID1 = 0x1BC;
//
//	sFilter2Config.FilterID2 = 0x2BC; /* For acceptance, MessageID and FilterID1 must match exactly */
//
//	/* Configure the FDCAN reception filter */
//	if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilter2Config) != HAL_OK) {
//		Error_Handler();
//	}
//
//	FDCAN_FilterTypeDef sFilter2Config1;
//	sFilter2Config1.IdType = FDCAN_STANDARD_ID;
//
//	sFilter2Config1.FilterIndex = 1;
//
//	sFilter2Config1.FilterType = FDCAN_FILTER_RANGE;
//
//	sFilter2Config1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//
//	sFilter2Config1.FilterID1 = 0X1BD;
//
//	sFilter2Config1.FilterID2 = 0X1BF; /* For acceptance, MessageID and FilterID1 must match exactly */
//	if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilter2Config1) != HAL_OK) {
//		Error_Handler();
//	}
//
//	FDCAN_FilterTypeDef sFilter2Config2;
//	sFilter2Config2.IdType = FDCAN_STANDARD_ID;
//
//	sFilter2Config2.FilterIndex = 2;
//
//	sFilter2Config2.FilterType = FDCAN_FILTER_RANGE;
//
//	sFilter2Config2.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//
//	sFilter2Config2.FilterID1 = 0X1C0;
//
//	sFilter2Config2.FilterID2 = 0x1C5; /* For acceptance, MessageID and FilterID1 must match exactly */
//	if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilter2Config2) != HAL_OK) {
//		Error_Handler();
//	}
//	FDCAN_FilterTypeDef sFilter2Config3;
//	sFilter2Config3.IdType = FDCAN_STANDARD_ID;
//
//	sFilter2Config3.FilterIndex = 3;
//
//	sFilter2Config3.FilterType = FDCAN_FILTER_DUAL;
//
//	sFilter2Config3.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//
//	sFilter2Config3.FilterID1 = 0x63C;
//
//	sFilter2Config3.FilterID2 = 0x5BC; /* For acceptance, MessageID and FilterID1 must match exactly */
//	if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilter2Config3) != HAL_OK) {
//		Error_Handler();
//	}
//	/* Start the FDCAN module */
//	if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	/* Enable interrupts */
//	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_FIFO_EMPTY, 0) != HAL_OK) {
//		/* Notification Error */
//		Error_Handler();
//	}
//	/* USER CODE END FDCAN2_Init 2 */
//
//}
//
//static uint32_t HAL_RCC_FDCAN_CLK_ENABLED=0;
//
//void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
//{
//
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	if(fdcanHandle->Instance==FDCAN1)
//	{
//		/* USER CODE BEGIN FDCAN1_MspInit 0 */
//
//		/* USER CODE END FDCAN1_MspInit 0 */
//		/* FDCAN1 clock enable */
//		HAL_RCC_FDCAN_CLK_ENABLED++;
//		if(HAL_RCC_FDCAN_CLK_ENABLED==1){
//			__HAL_RCC_FDCAN_CLK_ENABLE();
//		}
//
//		__HAL_RCC_GPIOA_CLK_ENABLE();
//		/**FDCAN1 GPIO Configuration
//    PA11     ------> FDCAN1_RX
//    PA12     ------> FDCAN1_TX
//		 */
//		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
//		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//		GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
//		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//		/* FDCAN1 interrupt Init */
//		HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
//		HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
//		/* USER CODE BEGIN FDCAN1_MspInit 1 */
//
//		/* USER CODE END FDCAN1_MspInit 1 */
//	}
//	else if(fdcanHandle->Instance==FDCAN2)
//	{
//		/* USER CODE BEGIN FDCAN2_MspInit 0 */
//
//		/* USER CODE END FDCAN2_MspInit 0 */
//		/* FDCAN2 clock enable */
//		HAL_RCC_FDCAN_CLK_ENABLED++;
//		if(HAL_RCC_FDCAN_CLK_ENABLED==1){
//			__HAL_RCC_FDCAN_CLK_ENABLE();
//		}
//
//		__HAL_RCC_GPIOB_CLK_ENABLE();
//		/**FDCAN2 GPIO Configuration
//    PB12     ------> FDCAN2_RX
//    PB13     ------> FDCAN2_TX
//		 */
//		GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
//		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//		GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
//		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//		/* FDCAN2 interrupt Init */
//		HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
//		HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
//		/* USER CODE BEGIN FDCAN2_MspInit 1 */
//
//		/* USER CODE END FDCAN2_MspInit 1 */
//	}
//}
//
//void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
//{
//
//	if(fdcanHandle->Instance==FDCAN1)
//	{
//		/* USER CODE BEGIN FDCAN1_MspDeInit 0 */
//
//		/* USER CODE END FDCAN1_MspDeInit 0 */
//		/* Peripheral clock disable */
//		HAL_RCC_FDCAN_CLK_ENABLED--;
//		if(HAL_RCC_FDCAN_CLK_ENABLED==0){
//			__HAL_RCC_FDCAN_CLK_DISABLE();
//		}
//
//		/**FDCAN1 GPIO Configuration
//    PA11     ------> FDCAN1_RX
//    PA12     ------> FDCAN1_TX
//		 */
//		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);
//
//		/* FDCAN1 interrupt Deinit */
//		HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
//		/* USER CODE BEGIN FDCAN1_MspDeInit 1 */
//
//		/* USER CODE END FDCAN1_MspDeInit 1 */
//	}
//	else if(fdcanHandle->Instance==FDCAN2)
//	{
//		/* USER CODE BEGIN FDCAN2_MspDeInit 0 */
//
//		/* USER CODE END FDCAN2_MspDeInit 0 */
//		/* Peripheral clock disable */
//		HAL_RCC_FDCAN_CLK_ENABLED--;
//		if(HAL_RCC_FDCAN_CLK_ENABLED==0){
//			__HAL_RCC_FDCAN_CLK_DISABLE();
//		}
//
//		/**FDCAN2 GPIO Configuration
//    PB12     ------> FDCAN2_RX
//    PB13     ------> FDCAN2_TX
//		 */
//		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);
//
//		/* FDCAN2 interrupt Deinit */
//		HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
//		/* USER CODE BEGIN FDCAN2_MspDeInit 1 */
//
//		/* USER CODE END FDCAN2_MspDeInit 1 */
//	}
//}
