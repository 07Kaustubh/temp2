/*
 *  Cota_Handler.c
 *  Created on: Aug 8, 2024
 *  Author: Riya
 */
#include "Cota_Handler.h"
#include "LC86.h"

extern char updateVersion[];
extern char LAC[];
extern ConfigItem configCotaTable[CONFIG_COUNT];
extern Stack packetHistoryQueue;
extern double latHitory, lonHistory;
extern uint64_t NPage, NBlock;
extern int8_t signalStrength;
extern RTC_TimeTypeDef sTime;
extern char Hardware_Version[], Firmware_Version[], Config_Version[];
bool SRESET_FLAG = false;
bool updateStatus;
//parse SMS
ConfigParametersID keyEnum;
ConfigParametersID getKeyEnum(const char *commandKey) {
	for (int i = 0; i < CONFIG_COUNT; i++) {
		if (commandKey != NULL) {
			if (strcmp(commandKey, configCotaTable[i].keyString) == 0) {
				return i;
			}
		}
	}
	return CONFIG_COUNT; // Invalid key
}

bool parseCommandSMS(char *SMSBuff, char *opBuffer) {
	//parse SMSBuff
	char splitBuffer[5][100] = {0};
	char TmpBuffer[200] = { 0 };
	splitString(SMSBuff, " ", splitBuffer, 4);
	if (splitBuffer[0][0] == '\0') {
		strcpy(opBuffer, "Didn't Get Proper command\n\0");
		return false;
	}
	char *command = (char*) strdup(splitBuffer[0]);
	if (splitBuffer[1][0] == '\0') {
		strcpy(opBuffer, "Didn't Got Any parameter1\n\0");
		return false;
	}
	char *parameter = strdup(splitBuffer[1]);
	memset(splitBuffer, 0, sizeof(splitBuffer));
	splitString(parameter, ":", splitBuffer, 3);
	parameter = strdup(splitBuffer[0]);
	char *value = strdup(splitBuffer[1]);

	if (strcmp(command, "SET") == 0) {
		if (parameter != NULL) {
			if (value != NULL) {
				keyEnum = getKeyEnum(parameter);
				if (keyEnum != CONFIG_COUNT) {
					setConfigValue(keyEnum, value);
					saveConfigToFlash();
					sprintf(opBuffer, "SET %s:%s OK ", parameter, value);
					printf("Set config %s to %s\n", parameter, value);
					if(strstr(parameter,"IMZ")){
						set_immobilization_tcp(true);
					}
					return true;
				} else if (strcmp(parameter, "UPFW") == 0) {
					sprintf(opBuffer, "\nDownloading Firmware -  %s\n", value);
					SET_FOTA_UPDATE(value);
					setUpdateStaus(true);
				} else if (strcmp(parameter, "RESET") == 0) {
					SRESET_FLAG = true;
					sprintf(opBuffer, "%s OK", parameter);
				}else if (strcmp(parameter, "HISTORYCLR") == 0) {
					clearHistory();
					sprintf(opBuffer, "%s OK", parameter);
				}
				else {
					printf("Invalid Config Key: %s\n", parameter);
					return false;
				}
			}
		}
	} else if (strcmp(command, "GET") == 0) {
		keyEnum = getKeyEnum(parameter);
		if (keyEnum != CONFIG_COUNT) {
			//const char *value = { 0 };
			value = getConfigValue(keyEnum);
			sprintf(opBuffer, "GET %s:%s OK ", parameter, value);
			printf("Config %s is %s\n", parameter, opBuffer);
			return true;
		} else if (strcmp(parameter, "DEVICEINFO") == 0) {
			getDeviceInfo(TmpBuffer);
			sprintf(opBuffer, "GET %s:%s  ", parameter, TmpBuffer);
		} else if (strcmp(parameter, "DEVICESTATUS") == 0) {
			getDeviceStatus(TmpBuffer);
			sprintf(opBuffer, "GET %s:%s  ", parameter, TmpBuffer);
		} else if (strcmp(parameter, "GPSINFO") == 0) {
			GetGps(TmpBuffer);
			sprintf(opBuffer, "GET %s:%s  ", parameter, TmpBuffer);
		} else if (strcmp(parameter, "DEVICEVERINFO") == 0) {
			GetVer(TmpBuffer);
			sprintf(opBuffer, "GET %s:%s  ", parameter, TmpBuffer);
		} else if (strcmp(parameter, "HISTORYCLR") == 0) {
			sprintf(TmpBuffer,"%d",(64 * NBlock)+NPage+packetHistoryQueue.top +1);
			sprintf(opBuffer, "History Total Packet:%s", TmpBuffer);
		}
		else {
			printf("Invalid config key: %s\n", parameter);
			return false;
		}
	} else {
		printf("Invalid command\n");
		return false;
	}
	return true;
}

//bool COTA_process(CotaType cotaType, char *cotaBuff, char *cotaResponseBuff) {
////SMSBuff="\0";
//
//	if (cotaType == COTA_SMS) {
//		return parseCommandSMS(cotaBuff, cotaResponseBuff);
//	}
//
//	else if (cotaType == COTA_UART) {
//		return parseCommandSMS(cotaBuff, cotaResponseBuff);
//	} else if (cotaType == COTA_TCP) {
//
//	}
//	return false;
//
//}

void SET_FOTA_UPDATE(char *Version) {
	strcpy(updateVersion, Version);
}
void SET_RESET() {
	saveConfigToFlash();
	HAL_NVIC_SystemReset();
}
bool getUpdateStatus() {
	return updateStatus;
}
void setUpdateStaus(bool upStatus) {
	updateStatus = upStatus;
}
extern struct GPSData *GPSDataTmp;
bool getDeviceInfo(char *infoBuffer) {
	// Prepare the formatted string with device information
	sprintf(infoBuffer, "IMEI-%s,ICCID-%s,IP1-%s,TCP1-%s,Ign-%d",
			getConfigValue(Config_IMEI), getConfigValue(Config_ICCID), getConfigValue(Config_MIP),
			getConfigValue(Config_MPORT1), getIgnition());
	return true;
}

bool getDeviceStatus(char *StatusBuffer) {
	// Retrieve configuration values
	float IPV = 0.0, EPV = 0.0;
	int HC = (64 * NBlock) + NPage; // Calculating History_Count
	char DeviceDate[9] = { 0 };
	char DeviceTime[9] = { 0 };
	// Retrieve date, time, battery, and voltage information
	getDateTime(DeviceDate, DeviceTime);
	getInternalBattery(&IPV);
	getInputVoltage(&EPV);
	// Prepare the formatted string with device status information
	sprintf(StatusBuffer,
			"SamTime- %s,RTC-%s,IBV-%.2f,EBV-%.2f,"
			"NetSth-%d,LAC-%s,History-%d",
	getConfigValue(Config_ION), DeviceTime, IPV, EPV, signalStrength, LAC,HC);
	return true;
}

bool GetGps(char *DataBuffer) {
	// Prepare the formatted string with GPS data
	sprintf(DataBuffer,"LAT-%.6f,LON-%.6f,Head-%d,Spd-%.2f,Alt-%d",
			latHitory, lonHistory, GPSDataTmp->heading, GPSDataTmp->speed,GPSDataTmp->alt);
	return true;
}

bool GetVer(char *DataBuffer) {
	// Prepare the formatted string with version information
	sprintf(DataBuffer,"FV- %s, CV- %s, HV - %s",
	Firmware_Version,getConfigValue(Config_CNF_VER), Hardware_Version);
	return true;
}


