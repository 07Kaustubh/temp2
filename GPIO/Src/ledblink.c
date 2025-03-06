/*
 * ledblink.c
 *
 *  Created on: May 17, 2024
 *      Author: 91944
 */
#include "ledblink.h"
#define HEIGH_TIME 8
#define LOW_TIME 1
LedHendler ledHendler = { .gpsLedState = LOW, .gsmLedState = LOW, .pwrLedState =
		SOLID, .isGpsLightON = false, .isGsmLightON = false, .isPwrLightON =
				false };
void togleState(bool *var) {
	*var = !(*var);
}
void handleGps() {
	if (ledHendler.isGpsLightON)
		GPS_LIGHT_ON();
	else
		GPS_LIGHT_OFF();
	togleState(&ledHendler.isGpsLightON);
}
void handleGsm() {
	if (ledHendler.isGsmLightON)
		GSM_LIGHT_ON();
	else
		GSM_LIGHT_OFF();
	togleState(&ledHendler.isGsmLightON);
}
void handlePwr() {
	if (ledHendler.isPwrLightON)
		PWR_LIGHT_ON();
	else
		PWR_LIGHT_OFF();
	togleState(&ledHendler.isPwrLightON);
}
void blinkLed(int time) {
	if ((time % HEIGH_TIME) == 0) {
		if (ledHendler.gpsLedState == HEIGH)
			handleGps();
		else if (ledHendler.gpsLedState == SOLID)
			GPS_LIGHT_ON();
		else if (ledHendler.gpsLedState == OFF)
			GPS_LIGHT_OFF();

		if (ledHendler.gsmLedState == HEIGH)
			handleGsm();
		else if (ledHendler.gsmLedState == SOLID)
			GSM_LIGHT_ON();
		else if (ledHendler.gsmLedState == OFF)
			GSM_LIGHT_OFF();
		if (ledHendler.pwrLedState == SOLID)
			PWR_LIGHT_ON();
		else if (ledHendler.pwrLedState == OFF)
			PWR_LIGHT_OFF();
		else if (ledHendler.pwrLedState == HEIGH)
			handlePwr();
	}
	if (time % LOW_TIME == 0) {
		if (ledHendler.gpsLedState == LOW)
			handleGps();
		if (ledHendler.gsmLedState == LOW)
			handleGsm();
		if (ledHendler.pwrLedState == LOW)
			handlePwr();
	}
}

void FUpdate() {
	ledHendler.pwrLedState=HEIGH;
	ledHendler.gsmLedState=OFF;
	ledHendler.gpsLedState=OFF;
}
void FDownload(){
	ledHendler.pwrLedState=LOW;
	ledHendler.gsmLedState=OFF;
	ledHendler.gpsLedState=OFF;
}
void FupdateC(){
	ledHendler.pwrLedState=SOLID;
	ledHendler.gsmLedState=SOLID;
	ledHendler.gpsLedState=SOLID;
	PWR_LIGHT_ON();
	GPS_LIGHT_ON();
	GSM_LIGHT_ON();
}
