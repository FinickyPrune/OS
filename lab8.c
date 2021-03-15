 #include <sys/types.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <errno.h>
 
 #define FAIL -1
 #define SUCCESS 0
 #define CHECK 0
 #define OPEN_ERROR -1
 #define FCNTL_ERROR -1
 #define LOCK_ERROR -1
 #define PRINTF_ERROR 0
 #define CLOSE_ERROR -1
 #define OPEN_EDITOR_ERROR -1
 #define SYSTEM_ERROR -1
 #define ALLOC_ERROR NULL

const char* editor_format_str = "vim %s";

int try_open_editor(const char* file_name)
{
    int length = snprintf(NULL, 0, editor_format_str, file_name);
    if (length < PRINTF_ERROR)
    {
        perror("Can't get result length");
        return OPEN_EDITOR_ERROR;
    } 

    length += 1;

    char* command = (char*)malloc(length * sizeof(*command));
    if (command == ALLOC_ERROR)
    {
        perror("Can't allocate memory for command");
        return OPEN_EDITOR_ERROR;
    }

    length = snprintf(command, length, editor_format_str, file_name);
    if (length < PRINTF_ERROR)
    {
        free(command);
        return OPEN_EDITOR_ERROR;
    }

    int system_check = CHECK; 
    system_check = system(command);
    if (system_check == SYSTEM_ERROR)
    {
        perror("System executed with error");
        free(command);
        return OPEN_EDITOR_ERROR;
    }
    
    free(command);
    return SUCCESS;
}

int advisory_lock(struct flock* lock, int file_descriptor, char* file_name)
{
    int fcntl_check = CHECK;
    lock->l_type = F_WRLCK;
 
    printf("Setting advisory lock\n");
 
    fcntl_check = fcntl(file_descriptor, F_SETLK, lock);
    if (fcntl_check == FCNTL_ERROR)
    {
        perror("fcntl() lock error");
        return SUCCESS;
    } 

    printf("Press Enter to open edtior\n");

    char enter_symbol = getchar();
    
    int check = CHECK;
    check = try_open_editor(file_name);
    if (check == OPEN_EDITOR_ERROR)
    {
        printf("Error while calling editor\n");
        return LOCK_ERROR;
    }

    lock->l_type = F_UNLCK;

    printf("Removing advisory lock\n");

    fcntl_check = fcntl(file_descriptor, F_SETLK, lock);
    if (fcntl_check == FCNTL_ERROR)
    {
        perror("fcntl() unlock error");
        return LOCK_ERROR;
    } 

    printf("Press Enter to continue...\n");

    enter_symbol = getchar();

    return SUCCESS;
}

int mandatory_lock(struct flock* lock, int file_descriptor, char* file_name)
{
    int fcntl_check = CHECK;
    lock->l_type = F_WRLCK;

    printf("Setting mandatory lock\n");

    fcntl_check = fcntl(file_descriptor, F_SETLKW, lock);
    if (fcntl_check == FCNTL_ERROR)
    {
        perror("fcntl() lock error");
        return SUCCESS;
    } 

    printf("Press Enter to open edtior\n");

    char enter_symbol = getchar();

    int check = CHECK;
    check = try_open_editor(file_name);
    if (check == OPEN_EDITOR_ERROR)
    {
        printf("Error while calling editor\n");
        return LOCK_ERROR;
    }

    lock->l_type = F_UNLCK;
    printf("Removing mandatory lock\n");
    
    fcntl_check = fcntl(file_descriptor, F_SETLKW, lock);
    if (fcntl_check == FCNTL_ERROR)
    {
        perror("fcntl() unlock error");
        return LOCK_ERROR;
    }

    return SUCCESS; 
}

 int main(int argc, char *argv[])
 {
    struct flock lock;
    
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    int file_descriptor = CHECK;
    int lock_check = CHECK;
    int close_check = CHECK;

    if (argc != 2)
    {
        printf("Usage: a.out f1\n");
        return FAIL;
    }
    
    file_descriptor = open(argv[1], O_RDWR);

    if (file_descriptor == OPEN_ERROR)
    {
        perror("Can't open file");
        return FAIL;
    }

    lock_check = advisory_lock(&lock, file_descriptor, argv[1]);
    if (lock_check == LOCK_ERROR)
    {
        printf("Error in advisory lock\n");
        close_check = close(file_descriptor);
        if (close_check == CLOSE_ERROR)
        {
            perror("Error with closing the file");
            return FAIL;
        }
        return FAIL;
    }

    lock_check = mandatory_lock(&lock, file_descriptor, argv[1]);
    if (lock_check == LOCK_ERROR)
    {
        printf("Error in mandatory lock\n");
        close_check = close(file_descriptor);
        if (close_check == CLOSE_ERROR)
        {
            perror("Error with closing the file");
            return FAIL;
        }

        return FAIL;
    }

    close_check = close(file_descriptor);
    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file");
        return FAIL;
    }

    return SUCCESS;
}
