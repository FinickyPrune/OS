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
#define NO_ARGUMENT NULL

int main(int argc, char* argv[])
{
    pid_t fork_check = CHECK_INIT;
    int status = 0;
    pid_t wait_check = CHECK_INIT;
    int execvp_check = CHECK_INIT;

    fork_check = fork();
    if (fork_check == FORK_ERROR)
    {
        perror("Error while creating process");
        return EXIT_FAILURE;
    }
    if (fork_check == CHILD_PROCESS)
    {
        if (argv[1] == NO_ARGUMENT)
        {
            printf("\n%s\n", "No command to pass to execvp.");
        }
        
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
   
    int exit_status = WEXITSTATUS(status); 
    printf("Exit status: %d\n", exit_status);
    return EXIT_SUCCESS;
}