#include <stdio.h>
#include <stdlib.h>

#ifndef UTIL_H
#define UTIL_H

// Prints an error of the format "error: <error message>" and exits the
// program with a non-zero error code
void 
exit_with_error(char *error) 
{
    fprintf(stderr, "Error: %s\n", error);
    exit(EXIT_FAILURE);
}

#endif