#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "process.h"
#include "CPU.h"
#include "CPU_manager.h"

#ifndef CUSTOM_SCHEDULER_H
#define CUSTOM_SCHEDULER_H

#define UNDEFINED -1
#define IS_PARALELLISABLE 'p'

int custom_scheduler(char *filename, int processors);

#endif
