/*
 * gpio_handler.h
 *
 *  Created on: May 9, 2024
 *      Author: 91944
 */

#ifndef INC_GPIO_HANDLER_H_
#define INC_GPIO_HANDLER_H_
#include "main.h"
#include "internalFlashStore.h"
#include "TML.h"

void config_IT();
void On_Wakeup_Config() ;
void Configure_RTC_WakeUp_Timer();
void Disable_EXTI0();
void SL_WK_GPIO_Int();
void Turn_OFF_Modules() ;
void GO_TO_SLEEP();
void Turn_ON_Modules();
void Sleep_Mode();
void WAKEUP();
void GO_TO_SLEEP();
void SL_WK_GPIO_Int();
bool setTime(uint8_t hours,uint8_t minute,uint8_t second);
bool setDate(uint8_t date,uint8_t month,uint8_t year);
bool getSleepStatus();
void setSleppStatus(bool sleep);
void initEvb(ADC_HandleTypeDef *adc);
void initIvb(ADC_HandleTypeDef *adc);
void initRTC(RTC_HandleTypeDef *hrtc) ;
void getDate(RTC_DateTypeDef *opDate);
void getTime(RTC_TimeTypeDef *opTime);
bool getDateTime(char *opDate, char *opTime);
bool getIgnition() ;
bool getInputVoltage(float *opADC);
bool getInternalBattery(float *opADC);
time_t convert_to_epoch(RTC_TimeTypeDef *time, RTC_DateTypeDef *date);
double get_Fotatime_difference(time_t start_epoch, time_t end_epoch) ;
bool getTemperStatus();
bool getDin1Status();
bool getDin2Status();
void ADC_Select_CH15();
void ADC_Select_CH1();
void set_immobilization();
void GO_TO_SLEEP_AFTER_BLE_SMS();
#endif /* INC_GPIO_HANDLER_H_ */
