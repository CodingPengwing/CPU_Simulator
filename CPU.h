#include "process.h"
#include "util.h"

#ifndef CPU_H
#define CPU_H

typedef struct cpu CPU;
struct cpu {
    unsigned int CPU_ID;
    unsigned int num_processes;
    Process *head;
    Process *tail;
};

CPU *
new_CPU() 
{
    CPU *object = (CPU*) malloc(sizeof(CPU));
    if (!object) exit_with_error("Error in new_CPU(): failed to malloc CPU");
    return object;
}

void
free_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in free_CPU(): pointer to cpu is NULL");
    Process *curr = cpu->head;
    while (curr->next) 
    {
        curr = curr->next;
        free(curr->prev);
    }
    free(curr);
    curr = NULL;
    cpu->head = NULL;
    cpu->tail = NULL;
    free(cpu);
}

void 
add_Process_to_CPU(CPU *cpu, Process *process) 
{
    if (cpu->num_processes)
    {
        process->next = cpu->head;
        cpu->head->prev = process;
        cpu->head = process;
    }
    else cpu->head = cpu->tail = process;
}

Process* 
pop_Process_from_CPU(CPU *cpu, Process *process) 
{
    if (!cpu || !process) exit_with_error("Error in pop_Process_from_CPU(): pointer given is NULL");
    if (!cpu->num_processes) exit_with_error("Error in pop_Process_from_CPU(): cannot pop an empty list");
    Process *process = cpu->head;
    if (cpu->num_processes > 1) 
    {
        cpu->head = cpu->head->next;
        cpu->head->prev = NULL;
    }
    else cpu->head = cpu->tail = NULL;
    cpu->num_processes--;
    return process;
}

void 
CPU_sort_processes() 
{
    
}


CPU **
new_CPU_manager() {}

void
free_CPU_manager() {}

void run_CPUs(CPU *CPU_array) 
{

}

void split_into_subprocesses(CPU **CPU_manager, Process* process) 
{

}

#endif