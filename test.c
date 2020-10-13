#include <signal.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 

void forkexample() 
{
    char *argv[2];
    // child process because return value zero
    int pid = fork();
    if (pid == 0) {
        printf("Hello from Child!\n"); 
        argv[0] = "./process";
        argv[1] = NULL;
        execvp(argv[0], argv);
    // parent process because return value non-zero. 
    } else {
        printf("Hello from Parent!\n");
        sleep(10);
        if (kill(pid, SIGINT)) {
            fprintf(stderr, "terminate of %d failed", (int) pid);
        }
    }
} 
int main() 
{ 
    forkexample(); 
    return 0; 
} 