
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#ifndef PROCESS_H
#define PROCESS_H

/* A Process is the lowest level data structure in this program. Processes are nodes
to a Queue. Queues can be part of CPUs. CPUs are part of CPU_Managers */
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
    unsigned int assigned_CPU;
    
    Process_t *next;
    Process_t *prev;

    Process_t *parent_process;
    unsigned int child_processes_remaining;
};

// Create and malloc a new process
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
    process->is_subprocess = false;
    process->next = process->prev = process->parent_process = NULL;

    return process;
}

// Print the process [FOR TESTING]
void
print_Process(Process_t *process) 
{
    if (!process) exit_with_error("Error in print_Process(): pointer to process is NULL.");
    printf("Process ID: %u | arrival time: %u | execution time: %u | remaining time: %u \n", 
    process->process_ID, process->time_arrived, process->execution_time,process->remaining_time);
}

// Print the RUNNING status for the process
void
print_Process_running(Process_t *process, unsigned int current_time) 
{
    if (!process) exit_with_error("Error in print_Process_running(): pointer given is NULL.");
    printf("%u,RUNNING,pid=%u", current_time, process->process_ID);
    if (process->is_subprocess) printf(".%u", process->subprocess_ID);
    printf(",remaining_time=%u,cpu=%u\n", process->remaining_time, process->assigned_CPU);
}

// Print the FINISHED status for the process
void
print_Process_finished(Process_t *process, unsigned int processes_remaining) 
{
    if (!process) exit_with_error("Error in print_Process_finished(): pointer given is NULL.");
    if (process->is_subprocess) exit_with_error("Error in print_Process_finished(): process given is a subprocess.");
    printf("%u,FINISHED,pid=%u,proc_remaining=%u\n", 
    process->time_finished, process->process_ID, processes_remaining);
}

// free the process
void 
free_Process(Process_t *process) 
{
    if (!process) exit_with_error("Error in free_Process(): pointer to process is NULL.");
    process->next = NULL;
    process->prev = NULL;
    process->parent_process = NULL;
    free(process);
}

// takes a specified execution time off of the current remaining_time for a Process
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

// This function is used to compare 2 Processes, starting with remaining execution time, 
// then process_ID, then subprocess_ID
int
compare_Processes(Process_t *process_1, Process_t *process_2) 
{
    if (!process_1 || !process_2) exit_with_error("Error in compare_Processes(): pointer to process is NULL.");

    if (process_1->remaining_time < process_2->remaining_time) return -1;
    if (process_1->remaining_time > process_2->remaining_time) return +1;
    
    if (process_1->process_ID < process_2->process_ID) return -1;
    if (process_1->process_ID > process_2->process_ID) return +1;

    if (process_1->subprocess_ID < process_2->subprocess_ID) return -1;
    if (process_1->subprocess_ID > process_2->subprocess_ID) return +1;

    exit_with_error("Error in compare_Processes(): processes shouldn't be equal.");
    return 0;
}

// This function is used to compare 2 Processes, configured specifically to work with the
// qsort() function.
int 
sort_Processes(const void *process_1, const void *process_2)
{
    // change the pointers to Process_t pointers to work with compare_Processes()
    Process_t **p_1 = (Process_t **) process_1;
    Process_t **p_2 = (Process_t **) process_2;
    return compare_Processes(*p_1, *p_2);
}

// This function is used to compare 2 Processes, configured specifically to work with the
// qsort() function. Sorts in biggest to smallest fashion.
int 
reverse_sort_Processes(const void *process_1, const void *process_2)
{
    // change the pointers to Process_t pointers to work with compare_Processes()
    Process_t **p_1 = (Process_t **) process_1;
    Process_t **p_2 = (Process_t **) process_2;
    return -compare_Processes(*p_1, *p_2);
}



#endif