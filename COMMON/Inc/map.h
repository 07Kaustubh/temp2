/*
 * map.h
 *
 *  Created on: Apr 29, 2024
 *      Author: 91944
 */

#ifndef INC_MAP_H_
#define INC_MAP_H_

#include"common.h"


#define MAX_MAP_DATA 50 // Maximum size of the stack
#define MAX_MAP_DATA_SIZE 9 // Size of the array to be pushed onto the stack
typedef struct  {
    uint32_t key;
    uint8_t value[MAX_MAP_DATA_SIZE]; // For simplicity, assuming values are integers
}KeyValuePair;

// Define a structure to represent the map
typedef struct  {
    KeyValuePair pairs[MAX_MAP_DATA];
    int size;
}Map;

#endif /* INC_MAP_H_ */
