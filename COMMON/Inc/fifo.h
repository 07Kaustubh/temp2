/*
 * fifo.h
 *
 *  Created on: Apr 26, 2024
 *      Author: CEINFO
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include<string.h>

#define MAX_STRING_LENGTH 1024 //max size of the FIFO queue
#define QUEUE_SIZE 50 // Maximum size of the queue

// Structure to represent a Queue
typedef struct {
	char items[QUEUE_SIZE][MAX_STRING_LENGTH];
    int front;
    int rear;
} Queue;

bool enqueue(Queue *q, const char *data);

bool dequeue(Queue *q,char *data);
#endif
