#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TRUE 1
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
#define TABLE_SIZE 256
#define CONSOLE_INPUT_SIZE 100

int printFile(size_t* line_lengths, off_t* file_offsets)
{
    int index = 1;
    int printf_check = 0;

    while (file_offsets[index] != 0)
    {
        
        printf_check = printf("%.*s", line_lengths[index], file_offsets[index]);
        if (printf_check < PRINTF_ERROR)
        {
            perror("Can't print file");
            return PRINT_FILE_ERROR;
        }
        fflush(stdout);
        index++;
    }

    return 0;
}

int fillTable(int file_descriptor, size_t* line_lengths, off_t* file_offsets, int file_size)
{
    int current_line_index = 1;
    int file_offset_index = 1;
    off_t current_offset = 0;
    char* file_adress = NULL;
    
    
    file_adress = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);    
    if (file_adress == MAP_FAILED)
    {
        perror("Can't map file");
        return FILL_TABLE_ERROR;
    }

    current_offset = (off_t)file_adress;

    for (int i = 0; i < file_size; i++)
    {
        line_lengths[current_line_index]++;
        current_offset++;
        if (*(file_adress + i) == '\n')
        {
            file_offsets[file_offset_index] = (off_t)(current_offset - line_lengths[current_line_index]);
            current_line_index++;
            file_offset_index++;
        }
    }
    return (current_line_index);
}

int getLines(size_t* line_lengths, off_t* file_offsets, int number_of_lines, int file_size)
{
    off_t line = 0;
    char console_input[CONSOLE_INPUT_SIZE]; 
    size_t line_number = 0;
    char enter_number_msg[23] = "Enter number of line: ";
    char timeout_warning_msg[31] = "Five seconds to enter number: ";
    char timeout_msg[26] = "\nTime is out. Your file:\n";
    int printf_check = 0;


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
        return GET_LINE_ERROR;
    }
    fflush(stdout);

    result = select(1, &read_descriptors, NULL, NULL, &timeout);

    if (result == SELECT_ERROR)
    {
        perror("Select error");
        return GET_LINE_ERROR;
    }

    if (result == 0)
    {
        int check = 0;

        printf_check = printf("%.*s", 26, timeout_msg);
        if (printf_check < PRINTF_ERROR)
        {
            perror("Can't print timeout message");
            return GET_LINE_ERROR;
        }
        fflush(stdout);

        check = printFile(line_lengths, file_offsets);
        if(check == PRINT_FILE_ERROR)
        {
            return GET_LINE_ERROR;
        }
        return 0;
    }

    while(TRUE)
    {
        int read_check = 0;
        read_check = read(STDIN_FILENO, console_input, CONSOLE_INPUT_SIZE);
        if (read_check == READ_ERROR)
        {
            perror("Can't read current text");
            return GET_LINE_ERROR;
        }

        char* endptr = NULL;
        line_number = strtoll(console_input, &endptr, 0);
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
            line = file_offsets[line_number];

            printf_check = printf("%.*s", line_lengths[line_number], line);
            if (printf_check < PRINTF_ERROR)
            {
                perror("Can't print line");
                return GET_LINE_ERROR;
            }
            fflush(stdout);

        } 
        printf_check = printf("%.*s", 23, enter_number_msg);
        if (printf_check < PRINTF_ERROR)
        {
            perror("Can't print \"Enter number\" message");
            return GET_LINE_ERROR;
        }
        fflush(stdout);
    }
    
    int munmap_check = 0;
    munmap_check = munmap((char*)file_offsets[1], file_size);
    if (munmap_check == MUNMAP_ERROR)
    {
        perror("Can't clean memory");
        return GET_LINE_ERROR;
    }
    return 0;
}

 int main(int argc, char* argv[])
 {
    off_t file_offsets[TABLE_SIZE]  = {0};
    size_t line_lengths[TABLE_SIZE]  = {0};

    size_t file_descriptor = -1;
    size_t number_of_lines = -1;
    struct stat file_info;
    int file_size = 0;
    
    if (argc != 2)
    {
        printf("Usage: a.out f1\n");
        return -1;
    }
    
    file_descriptor = open(argv[1], O_RDONLY);

    if (file_descriptor == OPEN_ERROR)
    {
        perror("Can't open file");
        return -1;
    }

    int fstat_check = 0;
    fstat_check = fstat(file_descriptor, &file_info);
    if (fstat_check == FSTAT_ERROR)
    {
        perror("Can't get file information");
        return -1;
    }
    file_size = (int)file_info.st_size;

    number_of_lines = fillTable(file_descriptor, line_lengths, file_offsets, file_size);
    
    if (number_of_lines == FILL_TABLE_ERROR)
    {
        printf("Error with filling the table");
        return -1;
    }
    
    int close_check = 1;
    close_check = close(file_descriptor);

    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file");
        return -1;
    }

    int get_lines_check = -1;
    
    get_lines_check = getLines(line_lengths, file_offsets, number_of_lines, file_size);
    
    if (get_lines_check == GET_LINE_ERROR)
    {
        printf("Error with printing lines\n");
        return -1;
    }
    return 0;
 }
