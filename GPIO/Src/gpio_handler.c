/*
 * gpio_handler.c
 *
 *  Created on: May 9, 2024
 *      Author: 91944
 */

#include"gpio_handler.h"

ADC_HandleTypeDef *evbSense, *ivbSense,*ainSense;
//#define REFERANCE_VOLTAGE 26.5
#define INTERNAL_BATTERY 1.935

//#define AIN_REFERANCE_VOLTAGE 0.6
//typedef enum {
//#define   CALIBRATION_0_8V  0.0   // 10-20V 0.453
//#define   CALIBRATION_8_20V  0.935  // 10-20V 0.453
//#define   CALIBRATION_20_30V  0.94 //	0.0674
//#define   CALIBRATION_30_40V  0.97  //   0.149
//#define   CALIBRATION_40_45V  0.99 // -0.9745
//#define   CALIBRATION_45_50V  1.05   //  -2.779 -4.817.-7.484 -10.466
//#define   CALIBRATION_50_55V  1.09
//#define   CALIBRATION_55_60V  1.15
//} CalibrationFactor;

//float getCalibrationFactor(float voltage) {
//	if (voltage >= 0.0 && voltage < 8.0) return CALIBRATION_0_8V;
//	else if (voltage >= 8.0 && voltage < 20.0) return CALIBRATION_8_20V;
//	else if (voltage >= 20.0 && voltage < 30.0) return CALIBRATION_20_30V;
//	else if (voltage >= 30.0 && voltage < 40.0) return CALIBRATION_30_40V;
//	else if (voltage >= 40.0 && voltage < 45.0) return CALIBRATION_40_45V;
//	else if (voltage >= 45.0 && voltage < 50.0) return CALIBRATION_45_50V;
//	else if (voltage >= 50.0 && voltage < 55.0) return CALIBRATION_50_55V;
//	else if (voltage >= 55.0 && voltage < 60.0) return CALIBRATION_55_60V;
//	return 1.0;  // Default calibration factor (no change)
//}
//#define CALIBRATION_FACTOR  0.93
bool sleepStatus = false,riStatusFlag=false;
extern bool bleSmsWakeupStatus;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim6;
extern int FLAG_RESET_On_Sleep;
extern uint8_t Dout1,Dout2;
extern uint64_t CP,CD;
extern uint8_t StrtFotaTmbuf[2], getFotaTmbuf[2];
extern char configValues[CONFIG_COUNT][LARGE_BUFFER_SIZE];
extern ConfigItem configCotaTable[CONFIG_COUNT] ;
extern bool sendPDDataFlag, PowerOnFlag, powerOnCount;
int FLAG_RESET_On_Sleep = 0;
uint32_t WRT_Time = 0x57E40; //0x36EE80;

void initEvb(ADC_HandleTypeDef *adc) {
	evbSense = adc;
}
void initIvb(ADC_HandleTypeDef *adc) {
	ivbSense = adc;
}
//void initAin(ADC_HandleTypeDef *adc){
//	ainSense = adc;
//}
//float raw = 0;
bool getInputVoltage(float *opADC) {
	// Wait for ADC conversion to complete
	//	ADC_Select_CH1();
	HAL_ADC_Start(evbSense);
	if (HAL_ADC_PollForConversion(evbSense, 2000) == HAL_OK) {
		// Read ADC value
		float raw =(float) HAL_ADC_GetValue(evbSense);
//		*opADC = (((raw) * 3.3 * 27.83) / ((1 << 12) - 1));
		*opADC=(raw*3.3*27.83/4095)-(raw/1500);
		// Stop ADC conversion
		HAL_ADC_Stop(evbSense);

		//		printc(MinDelay,"EXTERNAL VOLTAGE:[%.2f]\n",*opADC);
		return true;
	}
	return false;
}
bool getInternalBattery(float *opADC) {
	// Wait for ADC conversion to complete
	HAL_ADC_Start(ivbSense);
	if (HAL_ADC_PollForConversion(ivbSense, 2000) == HAL_OK) {
		// Read ADC value
		float raw =(float)HAL_ADC_GetValue(ivbSense);
		*opADC = (raw * 3.3 * INTERNAL_BATTERY) / ((1 << 12) - 1);
		// Stop ADC conversion
		HAL_ADC_Stop(ivbSense);
		//		printc(MinDelay,"INTERNAL BATTERY=>%d\n",raw);
		return true;
	}
	return false;
}
//bool getAin1(float *opADC) {
//	uint16_t raw = 0;
//	// Wait for ADC conversion to complete
//	ADC_Select_CH15();
//	HAL_ADC_Start(ainSense);
//	if (HAL_ADC_PollForConversion(ainSense, 2000) == HAL_OK) {
//		// Read ADC value
//		raw = HAL_ADC_GetValue(ainSense);
//		*opADC = (raw * 3.3 * 5.7) / ((1 << 12) - 1);
//		// Stop ADC conversion
//		HAL_ADC_Stop(ainSense);
//		return true;
//	}
//	return false;
//}
/****************************ALL INput pin status here*********************/
bool getIgnition() {
	return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0);
}
bool getTemperStatus() {
	return LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_15);
}
bool getDin1Status(){
	return LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_2);
}
bool getDin2Status(){
	return LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_4);
}
/*************************************************************************/
void config_IT() {

	//HAL_NVIC_DisableIRQ(EXTI8_IRQn);
	//HAL_NVIC_SetPriority(EXTI8_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(EXTI15_IRQn);
	HAL_NVIC_SetPriority(EXTI15_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(TIM6_IRQn);
	HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(UART5_IRQn);
	HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
	HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(GPDMA1_Channel4_IRQn);

	HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_IRQn);
	HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);

}
void On_Wakeup_Config() {
	HAL_NVIC_EnableIRQ(EXTI8_IRQn);
	HAL_NVIC_SetPriority(EXTI8_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_IRQn);
	HAL_NVIC_SetPriority(EXTI15_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM6_IRQn);
	HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(UART5_IRQn);
	HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
	HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel4_IRQn);
	HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
	HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(RTC_IRQn);
	HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
}


void GO_TO_SLEEP() {
	if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0) == RESET) {
		setConfigValue(Config_SleepFlag, "T\0");
		if(bleSmsWakeupStatus){
			bleSmsWakeupStatus=false;
		}
		else{
			printc(MinDelay, "Sleep_routine\r\n");
			Sleep_routine();
		}
		printc(MinDelay, "Sleep_Mode\r\n");
		setConfigValue(Config_PowerFlag, "F\0");//powerflag flag
		saveConfigToFlash();
		HAL_NVIC_SystemReset();

	}

}
///////////////////////go to sleep aftyer ble//////////////////////////////////////
//void GO_TO_SLEEP_AFTER_BLE_SMS(){
//	if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0) == RESET){
//		printc(MinDelay, "Sleep_routine after BLE or SMS\r\n");
//		setConfigValue(Config_SleepFlag, "T\0");
//		printc(MinDelay, "Sleep_Mode\r\n");
//		setConfigValue(Config_PowerFlag, "F\0");//powerflag flag
//		saveConfigToFlash();
//		HAL_NVIC_SystemReset();
//	}
//}
/////////////////////////////////////////////////////////////
// Function to disable EXTI line 0 interrupt
void Disable_EXTI0() {
	// Disable EXTI0 interrupt
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	// Clear any pending EXTI0 interrupt
	LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_EXTI_LINE0);

}

void SL_WK_GPIO_Int() {

	LL_EXTI_InitTypeDef EXTI_InitStruct = { 0 };
	LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTA, LL_EXTI_EXTI_LINE0);

	EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
	EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
	LL_EXTI_Init(&EXTI_InitStruct);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
	NVIC_SetPriority(EXTI0_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(EXTI0_IRQn);
}
void set_immobilization(){
	if(Dout1==1)
		D1_ON();
	else
		D1_OFF();
	if(Dout2==1)
		D2_ON();
	else
		D2_OFF();
}
void Turn_OFF_Modules() {
	GPIO_3V3_OFF();
	GPS_LIGHT_OFF();
	PWR_LIGHT_OFF();
	GSM_LIGHT_OFF();
	//#ifndef V2
	//	GPIO_3V3_OFF();
	//	GPIO_LTE_OFF();
	//#endif
}

void Turn_ON_Modules() {
	GPIO_3V3_ON();
	Handle_LEDS();
	GPIO_LTE_ON();
}
void Sleep_Mode() {
	//	char SF[8] = { 0 };

	//	FLASH_Read_Buff(configCotaTable[Config_SleepFlag].address, SF, 8);
	if (!getIgnition()) {
		sendPDDataFlag = true;
		getinitFlashConfig();
		config_IT();
		SL_WK_GPIO_Int();
		Configure_RTC_WakeUp_Timer();
		__HAL_RTC_WAKEUPTIMER_ENABLE(&hrtc);
		Turn_OFF_Modules();
		riStatusFlag=false;
		HAL_PWR_EnableSleepOnExit();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

		setConfigValue(Config_SleepFlag, "F\0");
		Disable_EXTI0();
		printc(MinDelay, "\r\nWokeUp\r\n");
		setConfigValue(Config_PowerFlag, "F\0");
		if(riStatusFlag){
			bleSmsWakeupStatus=true;
		}
		riStatusFlag=false;
		//		getinitFlashConfig();
	}
	else
	{
		setConfigValue(Config_PowerFlag, "T\0");
	}
}
