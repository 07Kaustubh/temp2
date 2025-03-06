/*
 * internalFlashStore.h
 *
 *  Created on: Aug 8, 2024
 *      Author: Riya
 */
//	SET_SKEY3("0F402D78266379087F7C1E0A02AD1D6D");
//	SET_SKEY2("967B9E42EC8F3E5F3D2DE0A5003F9129");
//	SET_SKEY1("1D68A17E0EE8525CC9876BD573C9CDCC");

#ifndef INC_INTERNALFLASHSTORE_H_
#define INC_INTERNALFLASHSTORE_H_
#include "common.h"
#include "memorymap.h"



void clear_RConfiguration();
void getinitFlashConfig();
char *getConfigValue(ConfigParametersID key);
void saveConfigToFlash();
void setConfigValue(ConfigParametersID key, const char* value);
void setFlashInttoString();
void getFlashStringtoInt(void);
#endif /* INC_INTERNALFLASHSTORE_H_ */
