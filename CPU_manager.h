
#include "process.h"
#include "queue.h"
#include "CPU.h"
#include "util.h"
#include <math.h>
#include <stdbool.h>

#ifndef CPU_MANAGER_H
#define CPU_MANAGER_H

#define UNDEFINED -1

/* A CPU_manager manages all the CPUs that are in the program. Its functions run on a
higher level and control all of the CPUs at a time. It provides functionality to delegate 
Processes to CPUs and remove finished Processes from CPUs among other higher level tasks. */
typedef struct cpu_manager CPU_Manager;
struct cpu_manager
{
    unsigned int num_CPUs;
    CPU **CPUs;
};

// Create and malloc a new CPU_Manager
CPU_Manager *
new_CPU_Manager(unsigned int num_CPUs);

// free a CPU_Manager
void
free_CPU_manager(CPU_Manager *cpu_manager);

// run all of the CPUs in the system, if possible
void run_CPUs(CPU_Manager *cpu_manager, unsigned int current_time, unsigned int running_time);

// finds the CPU with the shortest remaining time for its current Process
long long int 
get_shortest_CPU_running_time(CPU_Manager *cpu_manager);

// finds the CPU with the lighest load currently (least total execution remaining time). If there are
// CPUs to be excluded from this search, the CPU_IDs are to be provided in the exclusion_list.
CPU *
find_lightest_load_CPU(CPU_Manager *cpu_manager, int *exclusion_list, unsigned int exclusion_list_len);

// Splits the given process into k subprocesses and assigns to CPUs 
// according to CPU_ID for k = 2 and lowest-load CPUs for k > 2.
void
split_into_subprocesses(CPU_Manager *cpu_manager, Process_t *process, unsigned int k) ;

// Removes all finished Processes in the current state out of the CPUs
void
pop_finished_processes(CPU_Manager *cpu_manager, Queue_t *finished_queue, unsigned int current_time, unsigned int *ptr_processes_remaining);

// assign a list of Processes to CPUs
void
assign_Process_Queue_to_CPUs(CPU_Manager *cpu_manager, Queue_t *pending_queue);

#endif
