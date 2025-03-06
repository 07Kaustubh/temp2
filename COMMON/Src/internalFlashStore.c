/*
 * internalFlashStore.c
 *
 *  Created on: Aug 8, 2024
 *      Author: Riya
 */

#include "internalFlashStore.h"

extern ConfigItem configCotaTable[CONFIG_COUNT] ;
extern char configValues[CONFIG_COUNT][LARGE_BUFFER_SIZE];
extern uint64_t NPage , NBlock ;
int updateTimeFota;
extern long int lastFotaTime;
extern int WAkeuptimer;
extern double latHitory;
extern double lonHistory;
//new function start
extern uint8_t Dout1,Dout2;
void clear_RConfiguration() {
	for (int i = 0; i < CONFIG_COUNT ;i++) {
		memset(configValues[i], 0, LARGE_BUFFER_SIZE);
	}
}
void getinitFlashConfig() {
	char flashTempBuff[33] = {0}; // Ensure this is large enough for the largest config value
	clear_RConfiguration();
	for (int cntr = 0; cntr < CONFIG_COUNT; cntr++) {
		if(!configCotaTable[cntr].address==0){
			FLASH_Read_Buff(configCotaTable[cntr].address, flashTempBuff, configCotaTable[cntr].size);
			if (flashTempBuff[0] == 0xFF) {
				strncpy(configValues[cntr], configCotaTable[cntr].defaultValue, configCotaTable[cntr].size);
			} else {
				strncpy(configValues[cntr], flashTempBuff, configCotaTable[cntr].size);
			}
			configValues[cntr][configCotaTable[cntr].size - 1] = '\0'; // Ensure null-termination
		}
	}
	//why handled seperately??
	setStaticKeys(configValues[Config_SKEY1], configValues[Config_SKEY2], configValues[Config_SKEY3]);
	setComKeys(configValues[Config_CKEY1], configValues[Config_CKEY2],configValues[Config_CKEY3],configValues[Config_CKEY4],
			configValues[Config_CKEY5], configValues[Config_CKEY6], configValues[Config_CKEY7], configValues[Config_CKEY8]);
	getFlashStringtoInt();//getting the values into int
}

void setConfigValue(ConfigParametersID key, const char* value) {
	//always null turminate the string value
	memset(configValues[key], 0, LARGE_BUFFER_SIZE);
	strncpy(configValues[key], value, LARGE_BUFFER_SIZE - 1);
	configValues[key][LARGE_BUFFER_SIZE-1] = '\0';
}

char *getConfigValue(ConfigParametersID key) {
	return configValues[key];
}

void saveConfigToFlash() {
	//   HAL_FLASH_Unlock();
	printc(500, "\nstart of save config\n");
	setFlashInttoString();//converting the parameters that was used as int
	HAL_FLASH_ERASE(SMS_COTA_SEC);
	for (int i = 0; i < CONFIG_COUNT; i++) {
		if(!configCotaTable[i].address==0){
			if(configCotaTable[i].size<=16){
				//change to common write function once tested
				Write_CFGFlash(configCotaTable[i].address, (uint64_t*) configValues[i]);
			}
			else
				HAL_FLASH_Write_Buff(configCotaTable[i].address, (uint64_t*) configValues[i],sizeof(configValues[i]));
		}
	}
	printc(500, "\nend of save config\n");
	//   HAL_FLASH_Lock();
}
//for data used for counting
void setFlashInttoString(){
	//add only the variable that gets updated in the code
	sprintf(configValues[Config_BlockCount], "%d", NBlock);
	sprintf(configValues[Config_PageCount], "%d", NPage);
	//	sprintf(configValues[Config_FOTATime], "%d", updateTimeFota);
	//add fotastarttime
	sprintf(configValues[Config_LastFotaTime], "%ld", lastFotaTime);
	//	sprintf(configValues[Config_WAKEUP_TIMER], "%d",WAkeuptimer);
	//	sprintf(configValues[Config_NSAM], "%d",SAMPLIMG_FREQUENCY);
	//		sprintf(configValues[Config_LAT],"%.7f",latHitory);
	//		sprintf(configValues[Config_LON],"%.7f",lonHistory);
}

void getFlashStringtoInt(void){
	updateTimeFota=atoi(configValues[Config_FOTATime]);
	lastFotaTime=atol(configValues[Config_LastFotaTime]);
	WAkeuptimer = atoi(configValues[Config_WAKEUP_TIMER]);
//	SAMPLING_TIME =atoi(configValues[Config_NSAM]);
	Dout1=atoi(configValues[Config_DOUT1]);
	Dout2=atoi(configValues[Config_DOUT2]);
	sscanf(configValues[Config_LAT],"%lf",&latHitory);
	sscanf(configValues[Config_LON],"%lf",&lonHistory);
	NPage=atoi(configValues[Config_PageCount]);
	NBlock=atoi(configValues[Config_BlockCount]);
}
