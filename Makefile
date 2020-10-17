all: process robin

process:
	gcc -Wall -o process sigtrap.c

robin:
	gcc -Wall -o robin pcb.c robin.c