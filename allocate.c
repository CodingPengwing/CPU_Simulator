#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "process.h"
#include "CPU.h"

#define F_PARAMETER "-f"
#define P_PARAMETER "-p"
#define C_PARAMETER "-c"

void
print_logistics(unsigned long int makespan, unsigned long int total_turnaround_time, 
                unsigned int total_processes, float max_overhead, long double total_overhead);

int 
main(int argc, char *argv[]) 
{
    char *filename;
    unsigned int processors;
    bool custom_scheduler;

    // CHANGE TO GETOPT
    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(argv[i], C_PARAMETER) == 0) {custom_scheduler = true;}
        if (i == argc-1) break;
        if (strcmp(argv[i], F_PARAMETER) == 0) {filename = argv[i+1];}
        if (strcmp(argv[i], P_PARAMETER) == 0) {processors = (unsigned int) strtoul(argv[i+1], NULL, 0);}
    }
    
    unsigned long int current_time = 0;
    unsigned long int total_turnaround_time = 0;
    unsigned int total_processes = 0;
    float max_overhead = 0;
    long double total_overhead = 0;
    unsigned int time_arrived, process_ID, execution_time;
    char parallelisable;

    Queue_t *pending_queue = new_Queue();
    Queue_t *finished_queue = new_Queue();


    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) exit_with_error("Error: couldn't read file!");
    CPU_Manager *cpu_manager = new_CPU_Manager(processors);
    
    while (fscanf(input_file, "%d %d %d %c", &time_arrived, &process_ID, &execution_time, &parallelisable) == 4) 
    {
        while (current_time != time_arrived)
        {
            while (pending_queue->size)
            {
                Process_t *pending_process = pop_Queue(pending_queue);
                assign_Process_to_CPU(cpu_manager->CPUs[0], pending_process);
            }
            while (finished_queue->size)
            {
                Process_t *finished_process = pop_Queue(finished_queue);
                unsigned int processes_remaining = get_processes_remaining(cpu_manager);
                print_Process_finished(finished_process, current_time, processes_remaining);

                // DO STATISTICS !!!
                free_Process(finished_process);
            }
            unsigned int time_difference = time_arrived - current_time;
            unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
            if (time_difference < running_time) running_time = time_difference;
            run_CPUs(cpu_manager, current_time, running_time);
            current_time += running_time;
            
            for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
            {
                CPU *curr = cpu_manager->CPUs[i];
                if (!curr->process_queue->size) continue;
                if (curr->process_queue->head->remaining_time) continue;
                Process_t *finished_process = pop_Process_from_CPU(curr);
                insert_Queue(finished_queue, finished_process);
            }
        }

        Process_t *new_process = new_Process(time_arrived, process_ID, execution_time);
        assign_Process_to_CPU(cpu_manager->CPUs[0], new_process);
    }

    while (pending_queue->size)
    {
        Process_t *pending_process = pop_Queue(pending_queue);
        assign_Process_to_CPU(cpu_manager->CPUs[0], pending_process);
    }

    while (get_processes_remaining(cpu_manager) || finished_queue->size)
    {
        while (finished_queue->size)
        {
            Process_t *finished_process = pop_Queue(finished_queue);
            unsigned int processes_remaining = get_processes_remaining(cpu_manager);
            print_Process_finished(finished_process, current_time, processes_remaining);

            // DO STATISTICS !!!
            free_Process(finished_process);
        }
        unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
        run_CPUs(cpu_manager, current_time, running_time);
        current_time += running_time;

        for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
        {
            CPU *curr = cpu_manager->CPUs[i];
            if (!curr->process_queue->size) continue;
            if (curr->process_queue->head->remaining_time) continue;
            Process_t *finished_process = pop_Process_from_CPU(curr);
            insert_Queue(finished_queue, finished_process);
        }
    }

    fclose(input_file);
    free_CPU_manager(cpu_manager);
    free_Queue(pending_queue);
    free_Queue(finished_queue);

    return EXIT_SUCCESS;
}




void
print_logistics(unsigned long int makespan, unsigned long int total_turnaround_time, 
                unsigned int total_processes, float max_overhead, long double total_overhead)
{}