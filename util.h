#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef UTIL_H
#define UTIL_H

// Prints an error of the format "error: <error message>" and exits the
// program with a non-zero error code
void 
exit_with_error(char *error);

// This function prints the logistic calculations of the program at the end of the cycle.
void
print_logistics(unsigned int makespan, unsigned long int total_turnaround, 
                unsigned int total_processes, float max_overhead, double total_overhead);

#endif
