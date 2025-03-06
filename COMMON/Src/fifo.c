/*
 * fifo.c
 *
 *  Created on: Apr 26, 2024
 *      Author: CEINFO
 */
#include "fifo.h"
#include <string.h>


#include <stdio.h>

// Function to initialize the queue
void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

// Function to check if the queue is full
int isFull(Queue *q) {
    return (q->rear == QUEUE_SIZE - 1);
}

int isEmpty(Queue *q) {
    return (q->front == -1);
}

// Function to enqueue an element into the queue
bool enqueue(Queue *q, const char *mqttString) {
    if (isFull(q)) {
        printf("Queue is full. Cannot enqueue %s\n", mqttString);
        return false;
    }
    if (isEmpty(q)) {
        q->front = 0;
    }
    q->rear++;
    strcpy(q->items[q->rear], mqttString);
    return true;
}


bool dequeue(Queue *q,char *data) {

    if (isEmpty(q)) {
//        printc(MinDelay ,"Queue is empty. Cannot dequeue.\n");
        return false;
    }
    strcpy(data, q->items[q->front]);
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {

        q->front++;
    }
    return true;
}
/*
int main() {
    Queue queue;
 initQueue(&queue);

    // Enqueuing data in a loop for 4 times
    for (int i = 0; i < 4; i++) {
        int data;
        printf("Enter data to enqueue: ");
        scanf("%d", &data);
        enqueue(&queue, data);
    }

    return 0;
}
*/


