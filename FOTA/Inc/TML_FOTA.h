/*
 * TML_FOTA.h
 *
 *  Created on: Apr 23, 2024
 *      Author: 91944
 */

#ifndef TML_FOTA_H_
#define TML_FOTA_H_

#include "main.h"
#include "internalFlashStore.h"
#include "ledblink.h"
#include "sms.h"
#include "FOTA.h"
#include "LC86.h"


void Config_Sectorupdate();
void WriteF_SKey();
void WriteF_CKey();
void WriteF_AppLoc() ;
bool FOTA_Download_check();
void FOTA_AfterUpdate_routine();
bool update();


#endif /* TML_FOTA_H_ */
