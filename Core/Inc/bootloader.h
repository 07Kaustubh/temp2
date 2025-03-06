/*
 * bootloader.h
 *
 *  Created on: Oct 16, 2020
 *      Author: vikto
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_




#define L1_v1 ((uint32_t)0x08000000)
#define L2_v1 ((uint32_t)0x08020000)
#define L3_v1 ((uint32_t)0x08040000)

#define L2_v2 ((uint32_t)0x08040000)
#define L3_v2 ((uint32_t)0x08080000)



//#include "usbd_cdc_if.h"
#include <string.h>
#include "main.h"
#include "memorymap.h"
#include "Flash.h"
typedef void (application_t)(void);

typedef struct
{
    uint32_t		stack_addr;     // Stack Pointer
    application_t*	func_p;        // Program Counter
} JumpStruct;

void setupBootLoader();

uint32_t readWord(uint32_t address);

void jumpToApp();
void deinitEverything();
void  Application_jump();

#endif /* INC_BOOTLOADER_H_ */
