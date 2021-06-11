# This file is referenced from sample files provided to students as part of project 2 Comp30023 Melbourne University 2021.
# This file contains all the logic required to compile and run the program, as well as clean afterwards.

# CC - compiler
CC=gcc
# OBJ - compiled source files that should be linked
OBJ=process.o queue.o CPU.o CPU_manager.o util.o custom_scheduler.o
# COPT - compiler flags
COPT=-Wall -Wpedantic -g 
# BIN - binary
BIN_MAIN=allocate

# Running "make" with no argument will make the first target in the file
all: $(BIN_MAIN)

# Rules of the form
#     target_to_be_made : dependencies_to_be_up-to-date_first
#     <tab>commands_to_make_target
# (Note that spaces will not work.)

$(BIN_MAIN): $(BIN_MAIN).c $(OBJ)
	$(CC) -o $(BIN_MAIN) $(BIN_MAIN).c $(OBJ) $(COPT) -lm


# Wildcard rule to make any .o file,
# given a .c and .h file with the same leading filename component
%.o: %.c %.h
	$(CC) -c $< $(COPT) -g

format:
	clang-format -i *.c *.h

.PHONY: clean
clean:
	rm -rf $(BIN_MAIN) $(BIN_MAIN).exe *.o *.dSYM

