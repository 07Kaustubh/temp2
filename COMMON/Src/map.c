/*
 * map.c
 *
 *  Created on: Apr 29, 2024
 *      Author: 91944
 */


#include"map.h"

// Define a structure to hold key-value pairs


// Function to create a new ma

// Function to add a key-value pair to the map
void addToMap(Map* map, uint32_t  key, uint8_t *value,uint32_t size) {
    // Find an empty slot in the map
    int i;
    for (i = 0; i < map->size; i++) {
        if (map->pairs[i].key==key ) {
            clearBuffer(map->pairs[i].value, sizeof(map->pairs[i].value));
            memcpy(map->pairs[i].value,value,size);
//            printc(MinDelay,"CAN_DAta - %s", map->pairs[i].value);
            return;
        }
    }
    if(map->size >=MAX_MAP_DATA){
    	printc(MinDelay,"Map is full\n");
    }
    else{
    	clearBuffer(map->pairs[map->size].value, sizeof(map->pairs[map->size].value));
    	map->pairs[map->size].key=key;
    	memcpy(map->pairs[map->size].value,value,size);
    	map->size++;
    }

}

// Function to retrieve the value associated with a key
int getFromMap(Map* map, uint32_t key) {
    int i;
    for (i = 0; i < map->size; i++) {
        if (map->pairs[i].key != NULL && strcmp(map->pairs[i].key, key) == 0) {
            return map->pairs[i].value;
        }
    }
    printc(MinDelay,"Key not found\n");
    return -1; // Return a default value if key not found
}


// Function to free memory allocated for the map

//int main() {
//    struct Map* myMap = createMap(10); // Create a map with size 10
//    addToMap(myMap, "apple", 10);
//    addToMap(myMap, "banana", 20);
//
//    printf("Value for 'apple': %d\n", getFromMap(myMap, "apple"));
//    printf("Value for 'banana': %d\n", getFromMap(myMap, "banana"));
//    printf("Value for 'orange': %d\n", getFromMap(myMap, "orange"));
//
//    freeMap(myMap); // Free memory allocated for the map
//    return 0;
//}
