#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "process.h"
#include "CPU.h"
#include "CPU_manager.h"

#define F_PARAMETER "-f"
#define P_PARAMETER "-p"
#define C_PARAMETER "-c"
#define IS_PARALELLISABLE 'p'
#define UNDEFINED -1

void
print_logistics(unsigned int makespan, unsigned long int total_turnaround, 
                unsigned int total_processes, float max_overhead, double total_overhead);

int 
main(int argc, char *argv[]) 
{
    char *filename;
    unsigned int processors;
    bool custom_scheduler;

    // get command line input arguments
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
    unsigned int processes_remaining = 0;
    unsigned int *ptr_processes_remaining = &processes_remaining;
    unsigned int time_arrived, process_ID, execution_time;
    char parallelisable;

    // Queue for Processes pending to get added to a CPU
    Queue_t *pending_queue = new_Queue();
    // Queue for Processes that have finished in the previous state
    Queue_t *finished_queue = new_Queue();

    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) exit_with_error("Error: couldn't read file!");

    CPU_Manager *cpu_manager = new_CPU_Manager(processors);
    
    // Read all input and parse line by line
    while (fscanf(input_file, "%d %d %d %c", &time_arrived, &process_ID, &execution_time, &parallelisable) == 4) 
    {
        // Between the time a new process arrives and an old process arrived, we need to run simulations of
        // events that happened in that duration.
        while (current_time != time_arrived)
        {
            // assign any pending Processes to CPUs
            assign_Process_Queue_to_CPUs(cpu_manager, pending_queue);
            
            // if there are any finished processes, print the FINISHED status and keep track of statistics
            while (finished_queue->size)
            {
                Process_t *finished_process = pop_Queue(finished_queue);
                print_Process_finished(finished_process, processes_remaining);

                unsigned int turnaround_time = (finished_process->time_finished - finished_process->time_arrived);
                total_turnaround += turnaround_time;
                double overhead = (double)turnaround_time / finished_process->execution_time;
                total_overhead += overhead;
                if (overhead > max_overhead) max_overhead = overhead;

                free_Process(finished_process);
            }

            // calculate the amount of time the simulation should pass before the next event happens
            unsigned int time_difference = time_arrived - current_time;
            unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
            if (running_time == UNDEFINED || time_difference < running_time) running_time = time_difference;

            // execute the CPUs
            run_CPUs(cpu_manager, current_time, running_time);
            current_time += running_time;

            // find Processes that are finished
            pop_finished_processes(cpu_manager, finished_queue, current_time, ptr_processes_remaining);
        }

        // Adding processes to pending_queue
        Process_t *new_process = new_Process(time_arrived, process_ID, execution_time);
        total_processes++;
        processes_remaining++;

        // if it's not parallelsiable, insert straight away
        if (processors == 1 || parallelisable != IS_PARALELLISABLE)
        {
            insert_Queue(pending_queue, new_process);
            continue;
        }
        // if it is parallelisble, call split_into_subprocesses()
        else
        {
            unsigned int k = processors;
            if (execution_time >= 2 && execution_time < k) k = execution_time;
            split_into_subprocesses(cpu_manager, new_process, k);
        }        
    }

    // Repeat the above while loop until there are no more simulations to run
    assign_Process_Queue_to_CPUs(cpu_manager, pending_queue);
    while (processes_remaining || finished_queue->size)
    {   
        // Check for finished processes
        while (finished_queue->size)
        {
            Process_t *finished_process = pop_Queue(finished_queue);
            print_Process_finished(finished_process, processes_remaining);

            unsigned int turnaround_time = (finished_process->time_finished - finished_process->time_arrived);
            total_turnaround += turnaround_time;
            double overhead = (double)turnaround_time / finished_process->execution_time;
            total_overhead += overhead;
            if (overhead > max_overhead) max_overhead = overhead;

            free_Process(finished_process);
        }
        // run CPUs and pop finished processes
        unsigned int running_time = get_shortest_CPU_running_time(cpu_manager);
        if (running_time == UNDEFINED) break;
        run_CPUs(cpu_manager, current_time, running_time);
        current_time += running_time;
        pop_finished_processes(cpu_manager, finished_queue, current_time, ptr_processes_remaining);
    }

    print_logistics(current_time, total_turnaround, total_processes, max_overhead, total_overhead);
    fclose(input_file);
    free_CPU_manager(cpu_manager);
    free_Queue(pending_queue);
    free_Queue(finished_queue);
    return EXIT_SUCCESS;
}



// This function prints the logistic calculations of the program at the end of the cycle.
void
print_logistics(unsigned int makespan, unsigned long int total_turnaround, 
                unsigned int total_processes, float max_overhead, double total_overhead)
{
    unsigned int avg_turnaround =  (unsigned int) ceil((long double) total_turnaround / total_processes);
    double avg_overhead = (double) total_overhead / total_processes;

    // Rounding to 2 decimal places
    avg_overhead = (double)((int)(avg_overhead * 100 +.5))/100;
    max_overhead = (double)((int)(max_overhead * 100 +.5))/100;

    printf("Turnaround time %u\n", avg_turnaround);
    printf("Time overhead %g %g\n", max_overhead, avg_overhead);
    printf("Makespan %u\n", makespan);
}