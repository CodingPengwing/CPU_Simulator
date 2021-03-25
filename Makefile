allocate: allocate.c
	gcc -o allocate allocate.c

.PHONY: clean
clean:
	rm allocate

.PHONY: run
run:
	./allocate