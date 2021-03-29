
#include "process.h"
#include "queue.h"
#include "CPU.h"
#include "util.h"
#include <math.h>

#ifndef CPU_MANAGER_H
#define CPU_MANAGER_H

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

// free a CPU_Manager
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

// run all of the CPUs in the system, if possible
void run_CPUs(CPU_Manager *cpu_manager, unsigned int current_time, unsigned int running_time) 
{
    if (!cpu_manager) exit_with_error("Error in run_CPUs(): pointer given is NULL.");

    for (int i=0; i<cpu_manager->num_CPUs; i++) 
    {
        CPU *curr = cpu_manager->CPUs[i];
        // This is the process that is at the top of the CPU right now
        Process_t *top_process = curr->process_queue->head;
        if (top_process) 
        {
            // If the process is starting, print the RUNNING status message
            if (!top_process->is_running)
            {
                top_process->is_running = true;
                print_Process_running(top_process, current_time);
            }
            execute_Process(top_process, running_time);
            // Process has executed successfully without throwing errors
            // Update the load execution time for the CPU
            curr->load_remaining_time -= running_time;
        }
    }
}

// finds the CPU with the shortest remaining time for its current Process
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

// finds the CPU with the lighest load currently (least total execution remaining time). If there are
// CPUs to be excluded from this search, the CPU_IDs are to be provided in the exclusion_list.
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
    if (!lightest_cpu) exit_with_error("Error in find_lightest_load_CPU(): could not find CPU.");
    return lightest_cpu;
}

// Splits the given process into k subprocesses and assigns to CPUs 
// according to CPU_ID for k = 2 and lowest-load CPUs for k > 2.
void
split_into_subprocesses(CPU_Manager *cpu_manager, Process_t *process, unsigned int k) 
{
    if (!cpu_manager || !process) 
    exit_with_error("Error in split_into_subprocesses(): Pointed given is NULL.");
    if (k<2) exit_with_error("Error in split_into_subprocesses(): Cannot split process for given k value.");

    // Make a queue of subprocesses
    Queue_t *subprocesses = new_Queue();
    // Calculate the execution time for each child
    unsigned int execution_time = (int) ceil((double)process->execution_time/k) + 1;
    process->child_processes_remaining = k;

    // create subprocesses as new Processes
    for (int i=0; i<k; i++) 
    {
        Process_t *subprocess = new_Process(process->time_arrived, process->process_ID, execution_time);
        subprocess->is_subprocess = true;
        subprocess->parent_process = process;
        subprocess->subprocess_ID = i;
        insert_Queue(subprocesses, subprocess);
    }

    // If the number of CPUs is equal to 2, assign by CPU_ID
    if (cpu_manager->num_CPUs == 2) 
    {
        Process_t *subproc_1 = pop_Queue(subprocesses);
        assign_Process_to_CPU(cpu_manager->CPUs[0], subproc_1);
        Process_t *subproc_2 = pop_Queue(subprocesses);
        assign_Process_to_CPU(cpu_manager->CPUs[1], subproc_2);
    }
    // More than 2 CPUs, need to find the k lightest-load CPUs to assign to
    else
    {
        // Make an exclusion list for any CPUs that have already been assigned to
        int *exclusion_list = (int*) malloc(k * sizeof(int));
        for (int i=0; i<k; i++) exclusion_list[i] = -1;

        // Assign all subprocesses to unique CPUs
        while (subprocesses->size)
        {
            Process_t *subprocess = pop_Queue(subprocesses);
            // find the lighest-load CPU and assign to it
            CPU *cpu = find_lightest_load_CPU(cpu_manager, exclusion_list, k);
            assign_Process_to_CPU(cpu, subprocess);
            // add CPU_ID to exclusion list
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

// Removes all finished Processes in the current state out of the CPUs
void
pop_finished_processes(CPU_Manager *cpu_manager, Queue_t *finished_queue, unsigned int current_time, unsigned int *ptr_processes_remaining)
{
    for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
    {
        CPU *curr = cpu_manager->CPUs[i];
        // if CPU is empty, move on
        if (!curr->process_queue->size) continue;
        // if CPU process is still running, move on
        if (curr->process_queue->head->remaining_time) continue;
        // CPU process must be finished
        Process_t *finished_process = pop_Process_from_CPU(curr);
        finished_process->time_finished = current_time;

        // If it is not a subprocess, add straight to finished_queue
        if (!finished_process->is_subprocess)
        {
            insert_Queue(finished_queue, finished_process);
            *ptr_processes_remaining -= 1;
        }
        // If it is a subprocess, check whether the parent process is finished.
        else 
        {
            finished_process->parent_process->child_processes_remaining -= 1;
            // If parent process is finished, add parent process to the finished queue
            if (!finished_process->parent_process->child_processes_remaining)
            {
                finished_process->parent_process->time_finished = current_time;
                insert_Queue(finished_queue, finished_process->parent_process);
                *ptr_processes_remaining -= 1;
            }
            free_Process(finished_process);
        }
    }
}

// assign a list of Processes to CPUs
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