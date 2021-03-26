#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "process.h"

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

    // printf("filename:%s, processors: %d, custom scheduler: %s \n", filename, processors, custom_scheduler ? "true" : "false");

    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        fprintf(stderr, "Error: couldn't read file!");
        exit(EXIT_FAILURE);
    }

    unsigned int time_arrived, process_ID, execution_time;
    char parallelisable;

    while (fscanf(file, "%d %d %d %c", &time_arrived, &process_ID, &execution_time, &parallelisable) == 4) 
    {
        Process *process = new_Process(time_arrived, process_ID, execution_time);
        print_Process(process);
        execute_Process(process, 1);
        print_Process(process);
        free_Process(process);
        printf("-----\n");
    }

    return EXIT_SUCCESS;
}
