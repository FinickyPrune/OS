#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define FORK_ERROR -1
#define EXEC_ERROR -1
#define WAIT_ERROR -1
#define CHECK -1
#define CHILD_PROCESS 0
#define PARENT_PROCESS 1

int main(int argc, char* argv[])
{
    pid_t fork_check = CHECK;
    int status = 0;
    pid_t wait_check = 0;

    if (argc != 2)
    {
        printf("Usage: a.out f1\n");
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
        printf("\nI am a new process\nMy PID: %d\nMy PPID:%d\n",getpid(),getppid());
        int execl_check = CHECK;
        execl_check = execl("/bin/cat","cat",argv[1], NULL);
        if(execl_check == EXEC_ERROR)
        {
            perror("Error while openning cat");
            return EXIT_FAILURE;
        }
    }
    if (fork_check >= PARENT_PROCESS)
    {
        printf("I am parent\nMy child's PID: %d\nMy PID: %d\n", fork_check, getpid());
    }

    wait_check = wait(&status);
    if (wait_check == WAIT_ERROR)
    {
        perror("Error while waiting child determination");
        return EXIT_FAILURE;
    }

    printf("\nWait - ok!\n");

    return EXIT_SUCCESS;
}
