
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#ifndef PROCESS_H
#define PROCESS_H

typedef struct process Process_t;
struct process 
{
    unsigned int time_arrived;
    unsigned int process_ID;
    unsigned int execution_time;

    unsigned int remaining_time;
    unsigned int time_finished;
    bool is_running;
    bool is_subprocess;
    unsigned int subprocess_ID;
    // unsigned int finish_time;
    
    Process_t *next;
    Process_t *prev;

    Process_t *parent_process;
    unsigned int child_processes_remaining;
};

Process_t * 
new_Process(unsigned int time_arrived, unsigned int process_ID, unsigned int execution_time) 
{
    Process_t *process = (Process_t *) malloc(sizeof(Process_t));
    if (!process) exit_with_error("Error in new_Process(): failed to malloc Process.");
    process->time_arrived = time_arrived;
    process->process_ID = process_ID;
    process->execution_time = execution_time;
    process->remaining_time = execution_time;
    process->is_running = false;
    process->next = process->prev = process->parent_process = NULL;

    return process;
}

// Process *
// new_subProcess(unsigned int time_arrived, unsigned int subprocess_ID, Process *parent_process) 
// {

// }

void
print_Process(Process_t *process) 
{
    if (!process) exit_with_error("Error in print_Process(): pointer to process is NULL.");
    printf("Process ID: %u | arrival time: %u | execution time: %u | remaining time: %u \n", 
    process->process_ID, process->time_arrived, process->execution_time,process->remaining_time);
}

void
print_Process_running(Process_t *process, unsigned int current_time, unsigned int CPU_ID) 
{
    if (!process) exit_with_error("Error in print_Process_running(): pointer given is NULL.");
    printf("%u,RUNNING,pid=%u,remaining_time=%u,cpu=%u\n", 
    current_time, process->process_ID, process->remaining_time, CPU_ID);
}


void
print_Process_finished(Process_t *process, unsigned int processes_remaining) 
{
    if (!process) exit_with_error("Error in print_Process_finished(): pointer given is NULL.");
    printf("%u,FINISHED,pid=%u,proc_remaining=%u\n", 
    process->time_finished, process->process_ID, processes_remaining);
}

void 
free_Process(Process_t *process) 
{
    if (!process) exit_with_error("Error in free_Process(): pointer to process is NULL.");
    process->next = NULL;
    process->prev = NULL;
    process->parent_process = NULL;
    free(process);
}

void 
execute_Process(Process_t *process, unsigned int running_time) 
{
    if (!process) exit_with_error("Error in execute_Process(): pointer to process is NULL.");
    if (process->remaining_time < running_time) 
    {
        exit_with_error("Error in execute_Process(): cannot execute for longer than remaining_time.");
    }
    process->remaining_time -= running_time;
}

int
compare_Processes(Process_t *process_1, Process_t *process_2) 
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