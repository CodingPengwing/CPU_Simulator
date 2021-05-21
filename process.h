
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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
new_Process(unsigned int time_arrived, unsigned int process_ID, unsigned int execution_time);

// Print the process [FOR TESTING]
void
print_Process(Process_t *process);

// Print the RUNNING status for the process
void
print_Process_running(Process_t *process, unsigned int current_time);

// Print the FINISHED status for the process
void
print_Process_finished(Process_t *process, unsigned int processes_remaining);

// free the process
void 
free_Process(Process_t *process);

// takes a specified execution time off of the current remaining_time for a Process
void 
execute_Process(Process_t *process, unsigned int running_time);

// This function is used to compare 2 Processes, starting with remaining execution time, 
// then process_ID, then subprocess_ID
int
compare_Processes(Process_t *process_1, Process_t *process_2);

// This function is used to compare 2 Processes, configured specifically to work with the
// qsort() function.
int 
sort_Processes(const void *process_1, const void *process_2);

// This function is used to compare 2 Processes, configured specifically to work with the
// qsort() function. Sorts in biggest to smallest fashion.
int 
reverse_sort_Processes(const void *process_1, const void *process_2);

#endif
