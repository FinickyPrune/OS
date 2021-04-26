#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define FORK_ERROR -1
#define EXEC_ERROR -1
#define WAIT_ERROR -1
#define CHECK_INIT 0
#define CHILD_PROCESS 0
#define NUM_OF_ARGS 2

int main(int argc, char* argv[])
{
    pid_t fork_check = CHECK_INIT;
    int status = 0;
    pid_t wait_check = CHECK_INIT;
    int execvp_check = CHECK_INIT;

    if (argc < NUM_OF_ARGS)
    {
        printf("%s\n", "Usage: a.out f1 /f2 ... fn/");
        return EXIT_FAILURE;
    }
        
    fork_check = fork();
    if (fork_check == FORK_ERROR)
    {
        perror("Error while creating process");
        return EXIT_FAILURE;
    }
    if (fork_check == CHILD_PROCESS)
    {
        execvp_check = execvp(argv[1], &argv[1]); 
        if(execvp_check == EXEC_ERROR)
        {
            perror("Error in execv");
            return EXIT_FAILURE;
        }
    }

    wait_check = wait(&status);
    if ( wait_check == WAIT_ERROR)
    {
        perror("Error while waiting child determination");
        return EXIT_FAILURE;
    }
   
    if (WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status); 
        printf("Exit status: %d\n", exit_status);
        return EXIT_SUCCESS;
    }
    
    if (WIFSIGNALED(status))
    {
        int signal_number = WTERMSIG(status); 
        printf("Terminated by signal. Signal number: %d\n", signal_number);
        if (WCOREDUMP(status))
        {
            printf("Core-file was created.\n");
        }
        return EXIT_SUCCESS;
    }

}
