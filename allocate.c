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

int main(int argc, char *argv[]) 
{
    char *filename;
    unsigned int processors;
    bool custom_scheduler;

    unsigned int current_time = 0;

    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(argv[i], C_PARAMETER) == 0) {custom_scheduler = true;}
        if (i == argc-1) break;
        if (strcmp(argv[i], F_PARAMETER) == 0) {filename = argv[i+1];}
        if (strcmp(argv[i], P_PARAMETER) == 0) {processors = (unsigned int) strtoul(argv[i+1], NULL, 0);}
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) exit_with_error("Error: couldn't read file!");
    
    unsigned int time_arrived, process_ID, execution_time;
    char parallelisable;


    // --- TESTING CPU AND PROCESS MODULES --- //

    CPU_Manager *cpu_manager = new_CPU_Manager(processors);
    CPU *cpu = cpu_manager->CPUs[0];

    while (fscanf(file, "%d %d %d %c", &time_arrived, &process_ID, &execution_time, &parallelisable) == 4) 
    {
        Process *process = new_Process(time_arrived, process_ID, execution_time);
        add_Process_to_CPU(cpu, process);
    }

    print_CPU(cpu);
    printf("----\n");

    run_CPUs(cpu_manager, 20);

    Process *curr;
    while (cpu->num_processes) 
    {
        curr = pop_Process_from_CPU(cpu);
        printf("popped: ");
        print_Process(curr);
    }

    free_CPU_manager(cpu_manager);

    

    // --------------------------------------- //

    return EXIT_SUCCESS;
}
