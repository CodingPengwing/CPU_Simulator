
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#ifndef PROCESS_H
#define PROCESS_H

typedef struct process Process;
struct process 
{
    unsigned int time_arrived;
    unsigned int process_ID;
    unsigned int execution_time;

    unsigned int remaining_time;
    bool is_subprocess;
    unsigned int subprocess_ID;
    // unsigned int finish_time;
    
    Process *next;
    Process *prev;

    Process *parent_process;
    unsigned int child_processes_remaining;
};

Process * 
new_Process(unsigned int time_arrived, unsigned int process_ID, unsigned int execution_time) 
{
    Process *process = (Process*) malloc(sizeof(Process));
    if (!process) exit_with_error("Error in new_Process(): failed to malloc Process.");
    process->time_arrived = time_arrived;
    process->process_ID = process_ID;
    process->execution_time = execution_time;
    process->remaining_time = execution_time;
    process->next = process->prev = process->parent_process = NULL;

    return process;
}

// Process *
// new_subProcess(unsigned int time_arrived, unsigned int subprocess_ID, Process *parent_process) 
// {

// }

void
print_Process(Process *process) 
{
    if (!process) exit_with_error("Error in print_Process(): pointer to process is NULL.");
    printf("Process ID: %u | arrival time: %u | execution time: %u | remaining time: %u \n", 
    process->process_ID, process->time_arrived, process->execution_time,process->remaining_time);
}

void 
free_Process(Process *process) 
{
    if (!process) exit_with_error("Error in free_Process(): pointer to process is NULL.");
    process->next = NULL;
    process->prev = NULL;
    process->parent_process = NULL;
    free(process);
}

void 
execute_Process(Process *process, unsigned int running_time) 
{
    printf("executing process for running time of %d \n", running_time);
    if (!process) exit_with_error("Error in execute_Process(): pointer to process is NULL.");
    if (process->remaining_time < running_time) 
    {
        exit_with_error("Error in execute_Process(): cannot execute for longer than remaining_time.");
    }
    process->remaining_time -= running_time;
}

int
compare_Processes(Process *process_1, Process *process_2) 
{
    if (!process_1 || !process_2) exit_with_error("Error in compare_Processes(): pointer to process is NULL.");

    if (process_1->remaining_time < process_2->remaining_time) return -1;
    if (process_1->remaining_time > process_2->remaining_time) return +1;
    
    if (process_1->process_ID < process_2->process_ID) return -1;
    if (process_1->process_ID > process_2->process_ID) return +1;

    exit_with_error("Error in compare_Processes(): processes shouldn't be equal.");
    return 0;
}



#endif