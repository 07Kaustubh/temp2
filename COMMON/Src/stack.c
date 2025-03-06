/*
 * Stack.c
 *
 *  Created on: Apr 29, 2024
 *      Author: CEINFO
 */
#include <stack.h>

// Function to initialize the stack
void initializeStack(Stack *stack, int max_stack_data, int max_data_size) {
    stack->top = -1; // Initialize top index to -1 (empty stack)
    stack->max_stack_data = max_stack_data;
    stack->max_data_size = max_data_size;

    // Allocate memory for the stack data
    stack->data = (uint8_t **)malloc(max_stack_data * sizeof(uint8_t *));
    for (int i = 0; i < max_stack_data; i++) {
        stack->data[i] = (uint8_t *)malloc(max_data_size * sizeof(uint8_t));
    }
}

// Function to check if the stack is full
int full(Stack *stack) {
    return stack->top == stack->max_stack_data - 1; // Stack is full if top is at max index
}

// Function to check if the stack is empty
int empty(Stack *stack) {
    return stack->top == -1; // Stack is empty if top is -1
}

// Function to push an array onto the stack
bool push(Stack *stack, const uint8_t *array,uint32_t size) {
    if (full(stack)) {
        printc(MinDelay,"Error: Stack overflow\n");
        return false; // Exit function if stack is full
    }
    if(strlen(array)<2){
    	printc(MinDelay,"ERROR: NUll string cant be added in Queue\n");
    	return false;
    }

    stack->top++; // Increment top index
    clearBuffer(stack->data[stack->top],sizeof(stack->data[stack->top]));
    memcpy(stack->data[stack->top], array, size); // Copy array to stack
    stack->data[stack->top][size] ='\0';
//    print("Pushed array onto the stack\n");
    return true;
}

// Function to pop an array from the stack
bool pop(Stack *stack, uint8_t *result) {
    if (empty(stack)) {
        printc(MinDelay,"Error: Stack underflow\n");
        return false; // Exit function if stack is empty
    }
    strcpy(result,stack->data[stack->top]);
    //memcpy(result, stack->data[stack->top], size); // Copy array from stack
    clearBuffer(stack->data[stack->top], strlen(stack->data[stack->top]));
    stack->top--; // Decrement top index
    return true;
}

