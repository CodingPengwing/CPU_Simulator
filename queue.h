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
new_Queue() 
{
    Queue_t *queue = (Queue_t*) malloc(sizeof(*queue));
    if (!queue) exit_with_error("Error in new_Queue(): failed to malloc.");
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

// Print the queue
void 
print_Queue(Queue_t *queue) 
{
    if (!queue) exit_with_error("Error in print_Queue(): pointer given is NULL.");
    Process_t *curr = queue->head;
    while (curr) 
    {
        print_Process(curr);
        curr = curr->next;
    }
}

// Add an element to the tail of a queue
void 
insert_Queue(Queue_t *queue, Process_t *process) 
{
    if (!queue) exit_with_error("Error in insert_Queue(): queue pointer is NULL.");
    if (!process) exit_with_error("Error in insert_Queue(): process pointer is NULL.");
    process->next = NULL;
    process->prev = NULL;

    if(queue->size) 
    {
        process->prev = queue->tail;
        queue->tail->next = process;
        queue->tail = process;
    } 
    else queue->head = queue->tail = process;

    queue->size++;
}

// Add an element to the head of a queue
void
insert_at_head_Queue(Queue_t *queue, Process_t *process)
{
    if (!queue) exit_with_error("Error in insert_at_head_Queue(): queue pointer is NULL.");
    if (!process) exit_with_error("Error in insert_at_head_Queue(): process pointer given is NULL.");
    process->next = NULL;
    process->prev = NULL;

    if(queue->size) 
    {
        process->next = queue->head;
        queue->head->prev = process;
        queue->head = process;
    } 
    else queue->head = queue->tail = process;

    queue->size++;
}

// Remove and return the head element from a queue
Process_t *
pop_Queue(Queue_t *queue) 
{
    if (!queue) exit_with_error("Error in pop_Queue()(): queue pointer is NULL.");
    if (queue->size == 0) 
        exit_with_error("Error in pop_Queue(): can't pop from empty queue.");
    
    Process_t *top_process = queue->head;

    if (queue->size > 1)
    {
        queue->head = queue->head->next;
        queue->head->prev = NULL;
    }
    else 
        queue->head = queue->tail = NULL;
    
    queue->size--;
    top_process->next = top_process->prev = NULL;
    return top_process;
}

// Free the memory allocated to a queue
void 
free_Queue(Queue_t *queue) 
{
    if (!queue) exit_with_error("Error in free_Queue(): queue pointer is NULL.");

    while(queue->size) 
    {
        Process_t *process = pop_Queue(queue);
        free_Process(process);
    }
    queue->head = NULL;
    queue->tail = NULL;
    free(queue);
}

// Sort the queue. This function uses the standard qsort() function to help sort Processes.
Queue_t *
sort_Queue(Queue_t *queue, int (*compare)(const void*, const void*))
{
    if (!queue) exit_with_error("Error in sort_Queue(): queue pointer is NULL.");

    // If the queue is 0 or 1 elements, it is already sorted
    unsigned int width = queue->size;
    if (width <= 1) return queue;

    // Create an array so we can use the qsort() function
    Process_t **array = (Process_t **) malloc(width * sizeof(Process_t*));

    // Assign the Processes to the array
    Process_t *curr = queue->head;
    for (int i=0; i<width; i++)
    {
        array[i] = curr;
        curr = curr->next;
    }

    qsort(array, width, sizeof(Process_t *), compare);

    // After sorting, we need to make sure that the pointers in the sorted list are correct.
    queue->head = array[0];
    queue->tail = array[width-1];
    queue->head->prev = NULL;
    queue->tail->next = NULL;

    // set next and prev pointers to Processes to the right and left of each Process.
    for (int i = 0; i < width-1; i++) array[i]->next = array[i+1];
    for (int i = width-1; i > 1; i--) array[i]->prev = array[i-1];

    free(array);
    return queue;
}

#endif