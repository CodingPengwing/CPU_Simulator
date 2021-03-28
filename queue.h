#include <stdlib.h>
#include <stdio.h>
#include "process.h"
#include "util.h"

#ifndef QUEUE_H
#define QUEUE_H

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
    if (!queue || !process) exit_with_error("Error in insert_Queue(): pointer given is NULL.");
    process->next = process->prev = NULL;

    if(queue->size) 
    {
        process->prev = queue->tail;
        queue->tail->next = process;
        queue->tail = process;
    } 
    else 
        queue->head = queue->tail = process;

    queue->size++;
}

void
insert_at_head_Queue(Queue_t *queue, Process_t *process)
{
    if (!queue || !process) exit_with_error("Error in insert_at_head_Queue(): pointer given is NULL.");
    process->next = process->prev = NULL;

    if(queue->size) 
    {
        process->next = queue->head;
        queue->head->prev = process;
        queue->head = process;
    } 
    else 
        queue->head = queue->tail = process;

    queue->size++;
}

// Remove and return the head element from a queue
Process_t *
pop_Queue(Queue_t *queue) 
{
    if (!queue) exit_with_error("Error in pop_Queue()(): pointer given is NULL.");
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
    if (!queue) exit_with_error("Error in free_Queue(): pointer given is NULL.");
    while(queue->size) 
    {
        Process_t *process = pop_Queue(queue);
        free_Process(process);
    }
    queue->head = NULL;
    queue->tail = NULL;
    free(queue);
}


#endif