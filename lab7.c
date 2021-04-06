#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define WAIT_REACTION_ERROR -1
#define GET_NUMBER_ERROR -1
#define OPEN_ERROR -1
#define READ_ERROR -1
#define PRINTF_ERROR 0
#define CLOSE_ERROR -1
#define GET_LINE_ERROR -1
#define FILL_TABLE_ERROR -1
#define PRINT_FILE_ERROR -1
#define MUNMAP_ERROR -1
#define SELECT_ERROR -1
#define FSTAT_ERROR -1
#define STOP_NUMBER 0
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0
#define TABLE_SIZE 1024
#define CONSOLE_INPUT_SIZE 100
#define FAIL -1
#define SUCCESS 0
#define DECIMAL_SYSTEM 0
#define INIT_CHECK 0
#define NO_REACTION 0
#define MAX_DP 1

int printFile(size_t* line_lengths, off_t* memory_offsets)
{
    int index = 1;
    int printf_check = INIT_CHECK;

    while (memory_offsets[index] != 0)
    {
        
        printf_check = printf("%.*s", line_lengths[index], memory_offsets[index]);
        if (printf_check < PRINTF_ERROR)
        {
            perror("Can't print file");
            return PRINT_FILE_ERROR;
        }
        fflush(stdout);
        index++;
    }

    return SUCCESS;
}

int fillTable(char* file_adress, size_t* line_lengths, off_t* memory_offsets, int file_size)
{
    int current_line_index = 1;
    int memory_offset_index = 1;
    off_t current_offset = 0;
   
    current_offset = (off_t)file_adress;

    for (int i = 0; i < file_size; i++)
    {
        line_lengths[current_line_index]++;
        current_offset++;
        if (*(file_adress + i) == '\n')
        {
            memory_offsets[memory_offset_index] = (off_t)(current_offset - line_lengths[current_line_index]);
            current_line_index++;
            memory_offset_index++;
        }
    }
    return (current_line_index);
}

int waitForReaction()
{
    char timeout_warning_msg[31] = "Five seconds to enter number: ";
    char timeout_msg[26] = "\nTime is out. Your file:\n";
    
    int printf_check = INIT_CHECK;

    fd_set read_descriptors;
    struct timeval timeout;
    int result;

    FD_ZERO(&read_descriptors);
    FD_SET(STDIN_FILENO, &read_descriptors);

    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    printf_check = printf("%.*s", 31, timeout_warning_msg);
    if (printf_check < PRINTF_ERROR)
    {
        perror("Can't print timeout warning message");
        return WAIT_REACTION_ERROR;
    }
    fflush(stdout);

    result = select(MAX_DP, &read_descriptors, NULL, NULL, &timeout);
    
    if (result == SELECT_ERROR)
    {
        perror("Select error");
        return WAIT_REACTION_ERROR;
    }

    if (result == NO_REACTION)
    {
        int check = INIT_CHECK;

        printf_check = printf("%.*s", 26, timeout_msg);
        if (printf_check < PRINTF_ERROR)
        {
            perror("Can't print timeout message");
            return WAIT_REACTION_ERROR;
        }
        fflush(stdout);
        return FALSE;
    }

    if (FD_ISSET(STDIN_FILENO, &read_descriptors) != FALSE)
    {
        return TRUE;
    }
    return FALSE;
}

long long getNumber()
{
        long long line_number;
        char console_input[CONSOLE_INPUT_SIZE]; 

        int read_check = INIT_CHECK;
        read_check = read(STDIN_FILENO, console_input, CONSOLE_INPUT_SIZE);
        if (read_check == READ_ERROR)
        {
            perror("Can't read current text");
            return GET_NUMBER_ERROR;
        }

        char* endptr = NULL;
        line_number = strtoll(console_input, &endptr, DECIMAL_SYSTEM);
        
        return line_number;
}


int getLines(char* file_adress, size_t* line_lengths, off_t* memory_offsets, int number_of_lines, int file_size)
{
    off_t line = 0;
    long long line_number = 0;
    int waiting_result = 0;
    int printf_check = INIT_CHECK;

    while(TRUE)
    {
        waiting_result = waitForReaction();
        if (waiting_result == WAIT_REACTION_ERROR)
        {
            return GET_LINE_ERROR;
        }
        if (waiting_result == FALSE)
        {
            int check = INIT_CHECK;
            check = printFile(line_lengths, memory_offsets);
            if(check == PRINT_FILE_ERROR)
            {
                return GET_LINE_ERROR;
            }
            return SUCCESS;
        }
        line_number = getNumber();

        if (line_number == GET_NUMBER_ERROR)
        {
            printf("%s", "Can't get number from user");
            return GET_LINE_ERROR;
        }
        if (line_number == LLONG_MAX || line_number == LLONG_MIN)
        {
            perror("Invalid line number");
            continue;
        }
        if (line_number == STOP_NUMBER)
        {
            break;
        }
        if (line_number > number_of_lines || line_number < 0)
        {
            printf_check = printf("%s", "Invalid line number: No such line in file\n");
            if (printf_check < PRINTF_ERROR)
            {
                perror("Can't print info");
                return GET_LINE_ERROR;
            }
            continue;
        } 
        if (line_lengths[line_number] != 0)
        {
            line = memory_offsets[line_number];

            printf_check = printf("%.*s", line_lengths[line_number], line);
            if (printf_check < PRINTF_ERROR)
            {
                perror("Can't print line");
                return GET_LINE_ERROR;
            }
            fflush(stdout);

        } 
        
    }
    return SUCCESS;
}

 int main(int argc, char* argv[])
 {
    
    struct rlimit lim;

    getrlimit(RLIMIT_DATA, &lim);

    off_t memory_offsets[TABLE_SIZE]  = {0};
    size_t line_lengths[TABLE_SIZE]  = {0};

    int file_descriptor = INIT_CHECK;
    int number_of_lines = INIT_CHECK;
    struct stat file_info;
    int file_size = 0;
    char* file_adress = NULL;
    if (argc != 2)
    {
        printf("Usage: a.out f1\n");
        return FAIL;
    }
    
    file_descriptor = open(argv[1], O_RDONLY);

    if (file_descriptor == OPEN_ERROR)
    {
        perror("Can't open file");
        return FAIL;
    }

    int fstat_check = INIT_CHECK;
    fstat_check = fstat(file_descriptor, &file_info);
    if (fstat_check == FSTAT_ERROR)
    {
        perror("Can't get file information");
        return FAIL;
    }
    file_size = (long)file_info.st_size;

    file_adress = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);    

    if (file_adress == MAP_FAILED)
    {
        perror("Can't map file");
        return FAIL;
    }

    number_of_lines = fillTable(file_adress, line_lengths, memory_offsets, file_size);
    
    if (number_of_lines == FILL_TABLE_ERROR)
    {
        printf("Error with filling the table");
        return FAIL;
    }
    
    int close_check = INIT_CHECK;
    close_check = close(file_descriptor);

    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file");
        return FAIL;
    }

    int get_lines_check = INIT_CHECK;
    
    get_lines_check = getLines(file_adress, line_lengths, memory_offsets, number_of_lines, file_size);
    
    if (get_lines_check == GET_LINE_ERROR)
    {
        printf("Error with printing lines\n");
        return FAIL;
    }

    int munmap_check = INIT_CHECK;
    munmap_check = munmap(file_adress, file_size);
    if (munmap_check == MUNMAP_ERROR)
    {
        perror("Can't clean memory");
        return FAIL;
    }
    return SUCCESS;
 }
