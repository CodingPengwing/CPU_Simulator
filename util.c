#include "util.h"

// Prints an error of the format "error: <error message>" and exits the
// program with a non-zero error code
void 
exit_with_error(char *error) 
{
    fprintf(stderr, "Error: %s\n", error);
    exit(EXIT_FAILURE);
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
