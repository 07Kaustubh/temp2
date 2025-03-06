/*
 * Stack.h
 *
 *  Created on: Apr 29, 2024
 *      Author: CEINFO
 */

#ifndef INC_STACK_H_
#define INC_STACK_H_

#include <stdint.h>
#include<common.h>

#define MAX_STACK_DATA 80 // Maximum size of the stack
#define MAX_DATA_SIZE 2020 // Size of the array to be pushed onto the stack

typedef struct {
    uint8_t **data; // Array of uint8_t arrays
    int top; // Index of the top element in the stack
    int max_stack_data; // Maximum size of the stack
    int max_data_size; // Size of the array to be pushed onto the stack
} Stack;

// Function prototypes
void initializeStack(Stack *stack, int max_stack_data, int max_data_size);
int Full(Stack *stack);
int Empty(Stack *stack);
bool pop(Stack *stack, uint8_t *result);
bool push(Stack *stack, const uint8_t *array,uint32_t size);


#endif /* INC_STACK_H_ */
