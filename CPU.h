#include "process.h"
#include "queue.h"
#include "util.h"

#ifndef CPU_H
#define CPU_H

typedef struct cpu CPU;
struct cpu 
{
    unsigned int CPU_ID;
    Queue_t *process_queue;
};

CPU *
new_CPU(unsigned int CPU_ID) 
{
    CPU *cpu = (CPU*) malloc(sizeof(CPU));
    if (!cpu) exit_with_error("Error in new_CPU(): failed to malloc CPU.");
    cpu->process_queue = new_Queue();
    cpu->CPU_ID = CPU_ID;
    return cpu;
}

void
free_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in free_CPU(): pointer to cpu is NULL");
    if (cpu->process_queue->size) fprintf(stderr, "Error in free_CPU(): freeing CPU with pending processes.");
    
    free_Queue(cpu->process_queue);
    cpu->process_queue = NULL;
    free(cpu);
}

void
print_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in print_CPU(): pointer to cpu is NULL");
    printf("CPU number: %u \n", cpu->CPU_ID);
    print_Queue(cpu->process_queue);
}

void 
assign_Process_to_CPU(CPU *cpu, Process_t *new_process) 
{
    if (!cpu || !cpu->process_queue || !new_process) 
    exit_with_error("Error in add_Process_from_CPU(): pointer given is NULL");

    new_process->next = new_process->prev = NULL;

    if (cpu->process_queue->size == 0) 
    {
        insert_Queue(cpu->process_queue, new_process);
        return;
    }

    if (compare_Processes(new_process, cpu->process_queue->tail) > 0)
    {
        insert_Queue(cpu->process_queue, new_process);
        return;
    }
    
    if (compare_Processes(new_process, cpu->process_queue->head) < 0) 
    {
        insert_at_head_Queue(cpu->process_queue, new_process);
        new_process->next->is_running = false;
        return;
    }

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

// Pops a process from the CPU queue without freeing it
Process_t * 
pop_Process_from_CPU(CPU *cpu) 
{
    if (!cpu) exit_with_error("Error in pop_Process_from_CPU(): pointer given is NULL.");
    if (!cpu->process_queue->size) exit_with_error("Error in pop_Process_from_CPU(): cannot pop an empty queue.");
    
    Process_t *process = pop_Queue(cpu->process_queue);
    process->next = process->prev = NULL;
    return process;
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

    for (unsigned int i=0; i<num_CPUs; i++) 
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
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++) 
    {
        free_CPU(cpu_manager->CPUs[i]);
        cpu_manager->CPUs[i] = NULL;
    }
    free(cpu_manager->CPUs);
    cpu_manager->CPUs = NULL;
    free(cpu_manager);
}

void run_CPUs(CPU_Manager *cpu_manager, unsigned int current_time, unsigned int running_time) 
{
    if (!cpu_manager) exit_with_error("Error in run_CPU(): pointer given is NULL.");
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++) 
    {
        CPU *curr = cpu_manager->CPUs[i];
        Process_t *top_process = curr->process_queue->head;
        if (top_process) 
        {
            if (!top_process->is_running)
            {
                top_process->is_running = true;
                print_Process_running(top_process, current_time, curr->CPU_ID);
            }
            execute_Process(top_process, running_time);
        }
    }
}

unsigned int
get_processes_remaining(CPU_Manager *cpu_manager) 
{
    if (!cpu_manager) exit_with_error("Error in get_processes_remaining(): pointer given is NULL.");
    unsigned int processes_remaining = 0;
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++) 
    {
        processes_remaining += cpu_manager->CPUs[i]->process_queue->size;
    }
    return processes_remaining;
}

long long int 
get_shortest_CPU_running_time(CPU_Manager *cpu_manager)
{
    if (!cpu_manager) exit_with_error("Error in get_shortest_CPU_running_time(): pointer given is NULL.");
    long long int shortest_time = -1;
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
    {
        CPU *curr = cpu_manager->CPUs[i];
        if (!curr->process_queue->size) continue;
        if (shortest_time != -1 && shortest_time >= curr->process_queue->head->remaining_time) continue;
            
        shortest_time = curr->process_queue->head->remaining_time;
    }
    return shortest_time;
}

void split_into_subprocesses(CPU **CPU_manager, Process_t *process) 
{

}




#endif