/*
 * ledblink.h
 *
 *  Created on: May 17, 2024
 *      Author: 91944
 */

#ifndef INC_LEDBLINK_H_
#define INC_LEDBLINK_H_
#include "EC200Utils.h"
typedef enum {
	HEIGH,
	LOW,
	OFF,
	SOLID
	}LEDSTATE;
typedef struct{
	LEDSTATE gpsLedState;
	LEDSTATE gsmLedState;
	LEDSTATE pwrLedState;
	bool isGpsLightON;
	bool isGsmLightON;
	bool isPwrLightON;
}LedHendler;

void blinkLed(int time) ;
void FDownload();
void FupdateC();
void FUpdate();
void handlePwr();
void handleGsm();
void handleGps();
void togleState(bool *var);
#endif /* INC_LEDBLINK_H_ */
