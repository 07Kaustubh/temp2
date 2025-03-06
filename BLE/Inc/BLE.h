/*
 * BLE.h
 *
 *  Created on: Sep 9, 2024
 *      Author: Shekhar Verma
 */

#ifndef BLE_H_
#define BLE_H_

#include "stdbool.h"
#include "EC200Utils.h"
#include "main.h"
#include "common.h"

#define START 1
#define STOP  0
typedef enum {
	readWriteNotiIndi = 58,
	readWriteNotify = 26,
	readWrite = 10,
	read = 2,
	indicateNotify = 48,
	indicate = 32,
	notify = 16,
	write = 8,
	DisableConfig =0
} CharacteristicConfig;

bool ble_write(char*sendData);
bool ble_read(uint8_t);
bool ble_init();
bool ble_service_save();
bool ble_advertise_switch(int );
bool ble_switch(int on_off);
bool ble_add_service(int serviceId);
bool ble_configure_characteristic_descriptors();
void extractBetweenQuotes(const char*, char *);
void parse_ble_message();
void set_ble_read();
bool get_ble_read();
//char* hex_to_string(const char* hex_input);
void hexToString(const char *hex, char *output);
void ble_sleep_init();
void check_ble_response();
void set_ble_status(bool setBLE);
bool get_ble_status();
bool get_ble_read();
void set_ble_read();
#endif /* BLE_H_ */
