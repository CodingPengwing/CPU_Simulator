#include "process.h"
#include "queue.h"
#include "util.h"
#include "math.h"

#ifndef CPU_H
#define CPU_H

typedef struct cpu CPU;
struct cpu 
{
    unsigned int CPU_ID;
    unsigned int load_remaining_time;
    Queue_t *process_queue;
};

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
    cpu->load_remaining_time += new_process->remaining_time;
    new_process->assigned_CPU = cpu->CPU_ID;

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

    for (int i=0; i<num_CPUs; i++) cpu_manager->CPUs[i] = new_CPU(i); 
    
    cpu_manager->num_CPUs = num_CPUs;
    
    return cpu_manager;
}

void
free_CPU_manager(CPU_Manager *cpu_manager) 
{
    if (!cpu_manager) exit_with_error("Error in free_CPU_manager(): pointer given is NULL.");
    for (int i=0; i<cpu_manager->num_CPUs; i++) 
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
    if (!cpu_manager) exit_with_error("Error in run_CPUs(): pointer given is NULL.");
    for (int i=0; i<cpu_manager->num_CPUs; i++) 
    {
        CPU *curr = cpu_manager->CPUs[i];
        Process_t *top_process = curr->process_queue->head;
        if (top_process) 
        {
            if (!top_process->is_running)
            {
                top_process->is_running = true;
                print_Process_running(top_process, current_time);
            }
            execute_Process(top_process, running_time);
            // Process has executed successfully without throwing errors
            curr->load_remaining_time -= running_time;
        }
    }
}

unsigned int
get_processes_remaining(CPU_Manager *cpu_manager) 
{
    if (!cpu_manager) exit_with_error("Error in get_processes_remaining(): pointer given is NULL.");
    unsigned int processes_remaining = 0;
    for (int i=0; i<cpu_manager->num_CPUs; i++) 
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
    for (int i=0; i<cpu_manager->num_CPUs; i++)
    {
        CPU *curr = cpu_manager->CPUs[i];
        if (!curr->process_queue->size) continue;
        if (shortest_time == -1 || shortest_time > curr->process_queue->head->remaining_time)
            shortest_time = curr->process_queue->head->remaining_time;
    }
    return shortest_time;
}

CPU *
find_lightest_load_CPU(CPU_Manager *cpu_manager, int *exclusion_list, unsigned int exclusion_list_len)
{
    if (!cpu_manager) exit_with_error("Error in find_lightest_load_CPU(): pointer to cpu_manager is NULL.");
    long long int lightest_load = -1;
    CPU *lightest_cpu = NULL;
    for (int i=0; i<cpu_manager->num_CPUs; i++)
    {
        CPU *curr = cpu_manager->CPUs[i];
        bool excluded = false;
        // Check whether this CPU has been excluded from the lightest_load search
        if (exclusion_list)
        {
            for (int i=0; i<exclusion_list_len; i++) 
            {
                if (exclusion_list[i] < 0) continue;
                if (curr->CPU_ID == exclusion_list[i]) 
                { excluded = true; break; }
            }
        }
        if (excluded) continue;

        // CPU has not been excluded, evaluate whether is has the lightest load
        if (lightest_load == -1 || lightest_load > curr->load_remaining_time)
        {
            lightest_load = curr->load_remaining_time;
            lightest_cpu = curr;
        }
    }
    return lightest_cpu;
}

// Splits the given process into k subprocesses and assigns to CPUs 
// according to CPU_ID for k = 2 and lowest-load CPUs for k > 2
void
split_into_subprocesses(CPU_Manager *cpu_manager, Process_t *process, unsigned int k) 
{
    if (!cpu_manager || !process) 
    exit_with_error("Error in split_into_subprocesses(): Pointed given is NULL.");
    if (k<2) exit_with_error("Error in split_into_subprocesses(): Cannot split process for given k value.");

    Queue_t *subprocesses = new_Queue();
    unsigned int execution_time = (int) ceil((double)process->execution_time/k) + 1;
    process->child_processes_remaining = k;
    for (int i=0; i<k; i++) 
    {
        Process_t *subprocess = new_Process(process->time_arrived, process->process_ID, execution_time);
        subprocess->is_subprocess = true;
        subprocess->parent_process = process;
        subprocess->subprocess_ID = i;
        subprocess->subproc_split_value = k;
        insert_Queue(subprocesses, subprocess);
    }

    if (k == 2) 
    {
        Process_t *subproc_1 = pop_Queue(subprocesses);
        assign_Process_to_CPU(cpu_manager->CPUs[0], subproc_1);
        Process_t *subproc_2 = pop_Queue(subprocesses);
        assign_Process_to_CPU(cpu_manager->CPUs[1], subproc_2);
    }
    else
    {
        int *exclusion_list = (int*) malloc(k * sizeof(int));
        for (int i=0; i<k; i++) exclusion_list[i] = -1;
        while (subprocesses->size)
        {
            Process_t *subprocess = pop_Queue(subprocesses);
            CPU *cpu = find_lightest_load_CPU(cpu_manager, exclusion_list, k);
            assign_Process_to_CPU(cpu, subprocess);
            for (int i=0; i<k; i++) 
            {
                if (exclusion_list[i] == -1) 
                {
                    exclusion_list[i] = cpu->CPU_ID;
                    break;
                }
            }
        }
        free(exclusion_list);
    }

    free_Queue(subprocesses);
}


void
pop_finished_processes(CPU_Manager *cpu_manager, Queue_t *finished_queue, unsigned int current_time, unsigned int *ptr_processes_remaining)
{
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
    {
        CPU *curr = cpu_manager->CPUs[i];
        if (!curr->process_queue->size) continue;
        if (curr->process_queue->head->remaining_time) continue;
        Process_t *finished_process = pop_Process_from_CPU(curr);
        finished_process->time_finished = current_time;
        if (finished_process->is_subprocess)
        {
            finished_process->parent_process->child_processes_remaining--;
            if (!finished_process->parent_process->child_processes_remaining)
            {
                finished_process->parent_process->time_finished = current_time;
                insert_Queue(finished_queue, finished_process->parent_process);
                *ptr_processes_remaining -= 1;
            }
            free_Process(finished_process);
        }
        else 
        {
            insert_Queue(finished_queue, finished_process);
            *ptr_processes_remaining -= 1;
        }
    }
}

void
assign_Process_Queue_to_CPUs(CPU_Manager *cpu_manager, Queue_t *pending_queue)
{
    pending_queue = sort_Queue(pending_queue);
    while (pending_queue->size)
    {
        Process_t *process = pop_Queue(pending_queue);
        CPU *cpu = find_lightest_load_CPU(cpu_manager, NULL, 0);
        assign_Process_to_CPU(cpu, process);
    }    
}


#endif