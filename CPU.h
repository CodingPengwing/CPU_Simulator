#include "process.h"
#include "queue.h"
#include "util.h"


#ifndef CPU_H
#define CPU_H

/* A CPU is a data_structure that reflects the CPU component we are simulating. It 
contains the queue of Processes, the total remaining execution time, and the CPU ID */
typedef struct cpu CPU;
struct cpu 
{
    unsigned int CPU_ID;
    unsigned int load_remaining_time;
    Queue_t *process_queue;
};


// Create and malloc a new CPU
CPU *
new_CPU(unsigned int CPU_ID);

// free a CPU
void
free_CPU(CPU *cpu);

// print the contents of a CPU (Processes)
void
print_CPU(CPU *cpu);

// assign a Process to a CPU. This function will ensure that the Process queue inside
// the CPU is always sorted in ascending order based on the comparions rules specified
// in the specifications and programmed into compare_Processes()
void 
assign_Process_to_CPU(CPU *cpu, Process_t *new_process);

// Pops a process from the CPU queue without freeing it. Returns the Process.
Process_t * 
pop_Process_from_CPU(CPU *cpu);

#endif
