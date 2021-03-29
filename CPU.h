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
new_CPU(unsigned int CPU_ID) 
{
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    if (!cpu) exit_with_error("Error in new_CPU(): failed to malloc CPU.");
    cpu->process_queue = new_Queue();
    cpu->load_remaining_time = 0;
    cpu->CPU_ID = CPU_ID;
    return cpu;
}

// free a CPU
void
free_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in free_CPU(): pointer to cpu is NULL");
    if (cpu->process_queue->size) fprintf(stderr, "Error in free_CPU(): freeing CPU with pending processes.");
    
    free_Queue(cpu->process_queue);
    cpu->process_queue = NULL;
    free(cpu);
}

// print the contents of a CPU (Processes)
void
print_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in print_CPU(): pointer to cpu is NULL");
    printf("CPU number: %u \n", cpu->CPU_ID);
    print_Queue(cpu->process_queue);
}

// assign a Process to a CPU. This function will ensure that the Process queue inside
// the CPU is always sorted in ascending order based on the comparions rules specified
// in the specifications and programmed into compare_Processes()
void 
assign_Process_to_CPU(CPU *cpu, Process_t *new_process) 
{
    if (!cpu || !cpu->process_queue || !new_process) 
    exit_with_error("Error in add_Process_from_CPU(): pointer given is NULL");

    new_process->next = new_process->prev = NULL;
    cpu->load_remaining_time += new_process->remaining_time;
    new_process->assigned_CPU = cpu->CPU_ID;

    // if the CPU is empty, we can just safely insert
    if (cpu->process_queue->size == 0) 
    {
        insert_Queue(cpu->process_queue, new_process);
        return;
    }
    // if the new_process is bigger than the biggest process currently, put it 
    // at the bottom of the queue
    if (compare_Processes(new_process, cpu->process_queue->tail) > 0)
    {
        insert_Queue(cpu->process_queue, new_process);
        return;
    }
    // if the new_process is smaller than the smaller process currently, put it
    // at the top of the queue
    if (compare_Processes(new_process, cpu->process_queue->head) < 0) 
    {
        insert_at_head_Queue(cpu->process_queue, new_process);
        new_process->next->is_running = false;
        return;
    }
    // if the new_process is somewhere in between, we find the location where
    // it fits in by traversing from the top until we find a node that is bigger
    // than new_process
    Process_t *curr = cpu->process_queue->head;
    while (curr->next)
    {
        if (compare_Processes(new_process, curr->next) < 0)
        {
            new_process->next = curr->next;
            curr->next->prev = new_process;
            new_process->prev = curr;
            curr->next = new_process;
            cpu->process_queue->size++;
            return;
        }
        curr = curr->next;
    }
    
    fprintf(stderr, "Error in add_Process_to_CPU(): failed to add process to CPU.");
}

// Pops a process from the CPU queue without freeing it. Returns the Process.
Process_t * 
pop_Process_from_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in pop_Process_from_CPU(): pointer given is NULL.");
    if (!cpu->process_queue->size) exit_with_error("Error in pop_Process_from_CPU(): cannot pop an empty queue.");
    
    Process_t *process = pop_Queue(cpu->process_queue);
    process->next = process->prev = NULL;
    return process;
}




#endif