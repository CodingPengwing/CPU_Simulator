#include <stdlib.h>
#include <stdio.h>
#include "process.h"
#include "util.h"

#ifndef QUEUE_H
#define QUEUE_H

/* A queue is a list object that is used as a priority queue for CPUs and a normal list 
for other operations. Queues will be used in this program to hold chains of processes. */
typedef struct queue Queue_t;
struct queue 
{
    Process_t *head;
    Process_t *tail;
    unsigned int size;
};

// Create a new empty queue and return a pointer to it
Queue_t *
new_Queue();

// Print the queue
void 
print_Queue(Queue_t *queue);

// Add an element to the tail of a queue
void 
insert_Queue(Queue_t *queue, Process_t *process);

// Add an element to the head of a queue
void
insert_at_head_Queue(Queue_t *queue, Process_t *process);

// Remove and return the head element from a queue
Process_t *
pop_Queue(Queue_t *queue);

// Free the memory allocated to a queue
void 
free_Queue(Queue_t *queue);

// Sort the queue. This function uses the standard qsort() function to help sort Processes.
Queue_t *
sort_Queue(Queue_t *queue, int (*compare)(const void*, const void*));

#endif
