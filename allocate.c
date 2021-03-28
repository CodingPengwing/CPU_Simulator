#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "process.h"
#include "CPU.h"
#include "math.h"

#define F_PARAMETER "-f"
#define P_PARAMETER "-p"
#define C_PARAMETER "-c"

void
print_logistics(unsigned int makespan, unsigned long int total_turnaround, 
                unsigned int total_processes, float max_overhead, double total_overhead);

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
    
    unsigned int current_time = 0;
    unsigned long int total_turnaround = 0;
    unsigned int total_processes = 0;
    double max_overhead = 0;
    double total_overhead = 0;
    unsigned int time_arrived, process_ID, execution_time;
    char parallelisable;

    Queue_t *pending_queue = new_Queue();
    Queue_t *finished_queue = new_Queue();


    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) exit_with_error("Error: couldn't read file!");
    CPU_Manager *cpu_manager = new_CPU_Manager(processors);
    
    while (fscanf(input_file, "%d %d %d %c", &time_arrived, &process_ID, &execution_time, &parallelisable) == 4) 
    {
        // printf("%u, %u, %u, %c", time_arrived, process_ID, execution_time, parallelisable);
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
                print_Process_finished(finished_process, processes_remaining);

                unsigned int turnaround_time = (finished_process->time_finished - finished_process->time_arrived);
                total_turnaround += turnaround_time;
                double overhead = (double)turnaround_time / finished_process->execution_time;
                total_overhead += overhead;
                if (overhead > max_overhead) max_overhead = overhead;

                free_Process(finished_process);
            }
            unsigned int time_difference = time_arrived - current_time;
            unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
            if (running_time == -1 || time_difference < running_time) running_time = time_difference;
            run_CPUs(cpu_manager, current_time, running_time);
            current_time += running_time;
            
            for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
            {
                CPU *curr = cpu_manager->CPUs[i];
                if (!curr->process_queue->size) continue;
                if (curr->process_queue->head->remaining_time) continue;
                Process_t *finished_process = pop_Process_from_CPU(curr);
                finished_process->time_finished = current_time;
                insert_Queue(finished_queue, finished_process);
            }
        }

        Process_t *new_process = new_Process(time_arrived, process_ID, execution_time);
        total_processes++;
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
            print_Process_finished(finished_process, processes_remaining);

            unsigned int turnaround_time = (finished_process->time_finished - finished_process->time_arrived);
            total_turnaround += turnaround_time;
            double overhead = (double)turnaround_time / finished_process->execution_time;
            total_overhead += overhead;
            if (overhead > max_overhead) max_overhead = overhead;

            free_Process(finished_process);
        }
        unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
        if (running_time == -1) break;
        run_CPUs(cpu_manager, current_time, running_time);
        current_time += running_time;

        for (unsigned int i=0; i<cpu_manager->num_CPUs; i++)
        {
            CPU *curr = cpu_manager->CPUs[i];
            if (!curr->process_queue->size) continue;
            if (curr->process_queue->head->remaining_time) continue;
            Process_t *finished_process = pop_Process_from_CPU(curr);
            finished_process->time_finished = current_time;
            insert_Queue(finished_queue, finished_process);
        }
    }

    print_logistics(current_time, total_turnaround, total_processes, max_overhead, total_overhead);

    fclose(input_file);
    free_CPU_manager(cpu_manager);
    free_Queue(pending_queue);
    free_Queue(finished_queue);

    return EXIT_SUCCESS;
}




void
print_logistics(unsigned int makespan, unsigned long int total_turnaround, 
                unsigned int total_processes, float max_overhead, double total_overhead)
{
    unsigned int avg_turnaround =  (unsigned int) ceil((long double) total_turnaround / total_processes);
    double avg_overhead = (double) total_overhead / total_processes;

    avg_overhead = (double)((int)(avg_overhead * 100 +.5))/100;
    max_overhead = (double)((int)(max_overhead * 100 +.5))/100;

    printf("Turnaround time %u\n", avg_turnaround);
    printf("Time overhead %.2lf %.2lf\n", max_overhead, avg_overhead);
    printf("Makespan %u\n", makespan);
}