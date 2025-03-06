/*
 * TML_FOTA.c
 *
 *  Created on: Apr 23, 2024
 *      Author: 91944
 */
#include "TML_FOTA.h"

extern char PHnum[10];
extern bool enable_Idle;
extern char configValues[CONFIG_COUNT][LARGE_BUFFER_SIZE];
extern char ComKeys[8][33];
bool fotaReqestKey() {
	if (EC200_HTTP_POST(1))
		return true;
	return false;
}

bool fotaUpdate() {
	return EC200_HTTP_POST(6);
}

bool COTA_Update() {
	return EC200_HTTP_POST(5);
}

bool update() {
	bool status = true;
	if (!((configValues[Config_SKEY1][1] == 255) || (configValues[Config_SKEY1][1] == '\0'))) {
		EC200_Init();
		FDownload();
		gnss_data_off();
		if ((configValues[Config_CKEY1][1] == 255)
				|| (configValues[Config_CKEY1][1] == '\0')) {
			if (!fotaReqestKey()) {
				if (PHnum[0] != '\0' || PHnum[0] != 255) {
					char SBf[] = { "FOTA_Download Failed" };
					SMS_FSend(PHnum, SBf);
				}
				status = false;
			}
			saveConfigToFlash();
		}

		if (!fotaUpdate()) {
			if (PHnum[0] != '\0' || PHnum[0] != 255) {
				char SBf[] = { "FOTA_Download Failed" };
				SMS_FSend(PHnum, SBf);
			}
			status = false;
		}
		else
		{
			if (PHnum[0] != '\0' && PHnum[0] != 255) {
				char SBf[] = { "FOTA_Download Complete" };
				SMS_FSend(PHnum, SBf);
				setConfigValue(Config_PHNUM, PHnum);
			}
		}
		FupdateC();
		FOTA_AfterUpdate_routine();

		if (status)
			handleFotaACK(Downloaded);
		else
			handleFotaACK(Permanent_failed);
	} else {
		if (PHnum[0] != '\0' || PHnum[0] != 255) {
			char SBf[] = { "FOTA_Download Failed" };
			SMS_FSend(PHnum, SBf);
		}
		status = false;
	}
	return status;
}
void FOTA_AfterUpdate_routine() {
	saveConfigToFlash();
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	enableIdle();
	gnss_data_on();
}
bool FOTA_Download_check() {
	bool fotaStatus = true;
	char *tempKey = { 0 };
	tempKey = getConfigValue(Config_SKEY1);
	//if(tempKey!=NULL) address getting null need to fix
	if (tempKey != NULL) {
		if (tempKey[0] != 255 && tempKey[0] != '\0') {
			//check if comkeys present else download
			tempKey = getConfigValue(Config_CKEY1);
			if (tempKey[0] == 255 || tempKey[0] == '\0') {
				if (!EC200_HTTP_POST(1)){
					fotaStatus = false;
					return fotaStatus;
				}
			}
			if(!EC200_HTTP_POST(3))
				fotaStatus = false;
			else
				fotaStatus = true;
			//			fotaStatus = true;		//why this?
		} else {
			printc(MinDelay, "Error:static key not available");	//error message
			fotaStatus = false;
		}
	} else {
		printc(MinDelay, "Error:static key not available");	//error message
		fotaStatus = false;
	}
	return fotaStatus;
}
