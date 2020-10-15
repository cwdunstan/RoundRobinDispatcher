/*
    COMP3520 Assignment2 - Round Robin Dispatcher

    usage:

        ./robin <TESTFILE>
        where <TESTFILE> is the name of a job list
*/

/* Include files */
#include "robin.h"

int main (int argc, char *argv[])
{
    /*** Main function variable declarations ***/

    FILE * input_list_stream = NULL;
    PcbPtr fcfs_queue = NULL;
    PcbPtr robin_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;
    int procs = 0;
    int time_quantum = 0;
    int quantum = 0;
    float totalW, totalT;  
//  1. Populate the FCFS queue

    if (argc <= 0)
    {
        fprintf(stderr, "FATAL: Bad arguments array\n");
        exit(EXIT_FAILURE);
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(input_list_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

//  2. Fill dispatcher queue
    while (!feof(input_list_stream)) {  
        process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d",
             &(process->arrival_time), 
             &(process->service_time)) != 2) {
            free(process);
            continue;
        }
	process->remaining_cpu_time = process->service_time;
        process->status = PCB_INITIALIZED;
        procs ++;
        fcfs_queue = enqPcb(fcfs_queue, process);
    }

//  3. Ask user to input time_quantum
    CHECKPOINT:
    printf("Please enter a value for time quantum: ");
    scanf("%d", &time_quantum);

    if (time_quantum < 1)
    {
        fprintf(stderr, "Sorry, the time quantum must be at least one.\n");
        goto CHECKPOINT;
    }

//  4. While there is a currently running process or either queue is not empty
    while (current_process || fcfs_queue || robin_queue)
    {
//      i. Unload any arrived pending processes from the Job Dispatch queue dequeue process 
//         from Job Dispatch queue and enqueue on Round Robin queue;
        if (fcfs_queue && fcfs_queue->arrival_time <= timer) 
        {
            process = deqPcb(&fcfs_queue);
            robin_queue = enqPcb(robin_queue, process);
        }

//      ii. If a process is currently running
        if (current_process)
        {
//          a. Decrease process remaining_cpu_time by quantum;
            current_process->remaining_cpu_time = current_process->remaining_cpu_time-quantum;
//          b. If times up
            if (current_process->remaining_cpu_time <= 0) 
            {
                // update wait and TaT values
                totalT += timer - current_process->arrival_time;
                totalW += timer - (current_process->arrival_time + current_process->service_time);
//              A. Terminate the process;
                terminatePcb(current_process);
//              B. Deallocate the PCB (process control block)'s memory
                free(current_process);
                current_process = NULL;
            }
//          c. else if other processes are waiting in Round Robin queue
            else if (robin_queue)
            {
//              A. Send SIGTSTP to suspend currently running process;
                suspendPcb(current_process);
//              B. Enqueue it back to the tail of Round Robin queue;
                robin_queue = enqPcb(robin_queue, current_process);
                current_process = NULL;                
            }
        }
//      iii. If no process is currently running and Round Robin queue is not empty
        if (!current_process && robin_queue)
        {
//          a. Dequeue a process from the head of Round Robin queue;
//          b.  If the process job is a suspended process
//          c. else start it (fork & exec)
//          d. Set the process as the currently running process;

            current_process = deqPcb(&robin_queue);
            startPcb(current_process);
        }
//      iv. Sleep for quantum may/may not be the same as time_quantum, and may need to be calculated based on different situations);
        if (current_process) {
            if (current_process->remaining_cpu_time < time_quantum) {
                quantum = current_process->remaining_cpu_time;
            }
            else {
                quantum = time_quantum;
            }
        }
        sleep(quantum);
//      v. Increment timer by quantum;
        timer+=quantum;
//      vi. Go back to 4.        
    }

//  3. Terminate the FCFS dispatcher
    fprintf(stderr,"\ntotal TaT: %f\n total Wait: %f\n", totalT, totalW);
    fprintf(stderr,"\nAvg TaT: %f\n Avg Wait: %f\n", (totalT/procs), (totalW/procs));

    exit(EXIT_SUCCESS);
}
