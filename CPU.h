#include "process.h"
#include "util.h"

#ifndef CPU_H
#define CPU_H

typedef struct cpu CPU;
struct cpu 
{
    unsigned int CPU_ID;
    unsigned int num_processes;
    Process *head;
    Process *tail;
};

CPU *
new_CPU(unsigned int CPU_ID) 
{
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    if (!cpu) exit_with_error("Error in new_CPU(): failed to malloc CPU.");
    cpu->CPU_ID = CPU_ID;
    cpu->num_processes = 0;
    cpu->head = cpu->tail = NULL;
    return cpu;
}

void
free_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in free_CPU(): pointer to cpu is NULL");
    if (cpu->num_processes) fprintf(stderr, "Error in free_CPU(): freeing CPU with pending processes.");
    
    cpu->head = cpu->tail = NULL;
    free(cpu);
}

void 
add_Process_to_CPU(CPU *cpu, Process *new_process) 
{
    if (!cpu || !new_process) exit_with_error("Error in pop_Process_from_CPU(): pointer given is NULL");

    if (cpu->num_processes == 0) 
    {
        cpu->head = cpu->tail = new_process;
        cpu->num_processes++;
        return;
    }
    
    if (compare_Processes(new_process, cpu->head) < 0) 
    {
        new_process->next = cpu->head;
        cpu->head->prev = new_process;
        cpu->head = new_process;
        cpu->num_processes++;
        return;
    }

    if (compare_Processes(new_process, cpu->tail) > 0) 
    {
        new_process->prev = cpu->tail;
        cpu->tail->next = new_process;
        cpu->tail = new_process;
        cpu->num_processes++;
        return;
    }

    Process *curr = cpu->head;
    while (curr->next)
    {
        if (compare_Processes(new_process, curr->next) < 0)
        {
            new_process->next = curr->next;
            curr->next->prev = new_process;
            new_process->prev = curr;
            curr->next = new_process;
            cpu->num_processes++;
            return;
        }
        curr = curr->next;
    }
    
    exit_with_error("Error in add_Process_to_CPU(): failed to add process to CPU.");
}

Process* 
pop_Process_from_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in pop_Process_from_CPU(): pointer given is NULL.");
    if (!cpu->num_processes) exit_with_error("Error in pop_Process_from_CPU(): cannot pop an empty cpu.");
    
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
print_CPU(CPU *cpu)
{
    if (!cpu) exit_with_error("Error in print_CPU(): pointer given is NULL.");
    printf("CPU number: %d \n", cpu->CPU_ID);
    Process *curr = cpu->head;
    while (curr) 
    {
        print_Process(curr);
        curr = curr->next;
    }
}






typedef struct cpu_manager CPU_Manager;
struct cpu_manager
{
    unsigned int num_CPUs;
    CPU **CPUs;
};

CPU_Manager *
new_CPU_Manager(unsigned int num_CPUs) 
{
    CPU_Manager *cpu_manager = (CPU_Manager*) malloc(sizeof(CPU_Manager));
    if (!cpu_manager) exit_with_error("Error in new_CPU_Manager(): failed to malloc cpu_manager.");

    cpu_manager->CPUs = (CPU**) malloc(sizeof(CPU*) * num_CPUs);
    if (!cpu_manager->CPUs) exit_with_error("Error in new_CPU_Manager(): failed to malloc CPUs.");

    for (unsigned int i=0; i < num_CPUs; i++) 
    { 
        cpu_manager->CPUs[i] = new_CPU(i); 
    }
    cpu_manager->num_CPUs = num_CPUs;
    
    return cpu_manager;
}

void
free_CPU_manager(CPU_Manager *cpu_manager) 
{
    if (!cpu_manager) exit_with_error("Error in free_CPU_manager(): pointer given is NULL.");
    for (unsigned int i=0; i < cpu_manager->num_CPUs; i++) 
    {
        free_CPU(cpu_manager->CPUs[i]);
        cpu_manager->CPUs[i] = NULL;
    }
    free(cpu_manager->CPUs);
    cpu_manager->CPUs = NULL;
    free(cpu_manager);
}

void run_CPUs(CPU_Manager *cpu_manager, unsigned int running_time) 
{
    if (!cpu_manager) exit_with_error("Error in run_CPU(): pointer given is NULL.");
    for (unsigned int i=0; i < cpu_manager->num_CPUs; i++) 
    {
        CPU *curr = cpu_manager->CPUs[i];
        Process *top_process = curr->head;
        if (top_process) execute_Process(top_process, running_time);
    }
}

void split_into_subprocesses(CPU **CPU_manager, Process* process) 
{

}




#endif