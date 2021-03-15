#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define OPEN_ERROR -1
#define READ_ERROR -1
#define WRITE_ERROR -1
#define CLOSE_ERROR -1
#define LSEEK_ERROR (off_t)-1
#define GET_LINE_ERROR -1
#define FILL_TABLE_ERROR -1
#define PRINT_FILE_ERROR -1
#define GET_NUMBER_ERROR -1
#define READ_LINE_ERROR -1
#define PRINT_LINE_ERROR -1
#define SELECT_ERROR -1
#define WAIT_REACTION_ERROR -1
#define STOP_NUMBER '0'
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0
#define TABLE_SIZE 256
#define CONSOLE_INPUT_SIZE 100
#define FAIL -1
#define SUCCESS 0
#define DECIMAL_SYSTEM 0
#define INIT_CHECK 0
#define BUFFER_SIZE 1024

int printFile(int file_descriptor)
{
    char* buffer[BUFFER_SIZE];

    int actual_buffer_size =  BUFFER_SIZE;

    off_t lseek_check  = INIT_CHECK;
    lseek_check = lseek(file_descriptor, 0, SEEK_SET);
    if (lseek_check == LSEEK_ERROR)
    {
        perror("Seek error");
        return PRINT_FILE_ERROR;
    }

    while (TRUE)
    {
        actual_buffer_size = read(file_descriptor, buffer, BUFFER_SIZE);
        if (actual_buffer_size == READ_ERROR)
        {
            perror("Can't read current text");
            return PRINT_FILE_ERROR;
        }

        if (actual_buffer_size == 0)
        {
            break;
        }
        
        int write_check = INIT_CHECK;
        write_check = write(STDOUT_FILENO, buffer, actual_buffer_size);
        if (write_check == WRITE_ERROR)
        {
            perror("Can't print message for user");
            return PRINT_FILE_ERROR;
        }

    }

}

int fillTable(int file_descriptor, size_t* line_lengths, off_t* file_offsets)
{
    char* read_buffer[BUFFER_SIZE];

    if (read_buffer == ALLOC_ERROR)
    {
        perror("Can't allocate memory foor buffer");
        return FILL_TABLE_ERROR;
    }
    int actual_buffer_size =  1;

    int current_line_index = 1;
    int file_offset_index = 1;
    int current_offset = 0;
    
    while (actual_buffer_size > 0)
    {
        actual_buffer_size = read(file_descriptor, read_buffer, BUFFER_SIZE);
        if (actual_buffer_size == READ_ERROR)
        {
            perror("Can't read current text");
            return FILL_TABLE_ERROR;
        }
        
        for (size_t i = 0; i < actual_buffer_size; i++)
        {
            line_lengths[current_line_index]++;
            current_offset++;
            if (read_buffer[i] == '\n')
            {
                file_offsets[file_offset_index] = (off_t)(current_offset - line_lengths[current_line_index]);
                current_line_index++;
                file_offset_index++;
            }
        }
    }

    return (current_line_index);
}

long long getNumber()
{
        char text_for_user[23] = "Enter number of line: ";
        long long line_number;
        char console_input[CONSOLE_INPUT_SIZE]; 

        int write_check = INIT_CHECK;
        write_check = write(STDOUT_FILENO, text_for_user, 23);
        if (write_check == WRITE_ERROR)
        {
            perror("Can't print message for user");
            return GET_NUMBER_ERROR;
        }
        
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

int readLineFromFile(int file_descriptor, int line_length, off_t offset, char* line)
{
        off_t lseek_check  = INIT_CHECK;
        lseek_check = lseek(file_descriptor, offset, SEEK_SET);

        if (lseek_check == LSEEK_ERROR)
        {
            perror("Seek error");
            return READ_LINE_ERROR;
        }
    
        int read_check = INIT_CHECK;
        read_check = read(file_descriptor, line, line_length);

        if (read_check == READ_ERROR)
        {
            perror("Can't read current text");
            return READ_LINE_ERROR;
        }

        return SUCCESS;
}

int printLine(char* line, int line_length)
{
    int write_check = INIT_CHECK;
    write_check = write(STDOUT_FILENO, line, line_length);
    if (write_check == WRITE_ERROR)
    {
        perror("Can't print line");
        return PRINT_LINE_ERROR;
    }

    return SUCCESS;
}

int waitForReaction(int file_descriptor)
{
    char timeout_warning_msg[31] = "Five seconds to enter number: ";
    char timeout_msg[26] = "\nTime is out. Your file:\n";
    fd_set read_descriptors;
    struct timeval timeout;
    int result;

    FD_ZERO(&read_descriptors);
    FD_SET(STDIN_FILENO, &read_descriptors);

    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    int write_check = INIT_CHECK;
    write_check = write(STDOUT_FILENO, timeout_warning_msg, 31);
    if (write_check == WRITE_ERROR)
    {
        perror("Can't print message for user");
        return WAIT_REACTION_ERROR;
    }
        
    result = select(1, &read_descriptors, NULL, NULL, &timeout);

    if (result == SELECT_ERROR)
    {
        perror("Select error");
        return WAIT_REACTION_ERROR;
    }

    if (result == 0)
    {
        int check = INIT_CHECK;
        write_check = write(STDOUT_FILENO, timeout_msg, 26);
        if (write_check == WRITE_ERROR)
        {
            perror("Can't print message for user");
            return WAIT_REACTION_ERROR;
        }
        return FALSE;
    }

    return TRUE;

}

int getLines(int file_descriptor, size_t* line_lengths, off_t* file_offsets, int number_of_lines)
{
    long long line_number = 0;
    off_t offset = 0;
    int line_length = 0;
    int waiting_result = 0;
   
    waiting_result = waitForReaction(file_descriptor);
    if (waiting_result == WAIT_REACTION_ERROR)
    {
        return GET_LINE_ERROR;
    }
    if (waiting_result == FALSE)
    {
        int check = INIT_CHECK;
        check = printFile(file_descriptor);
        if(check == PRINT_FILE_ERROR)
        {
            return GET_LINE_ERROR;
        }
        return SUCCESS;
    }
    
    while(TRUE)
    {
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
            printf("%s", "Invalid line number: No such line in file\n");
            continue;
        } 
        if (line_lengths[line_number] != 0)
        {
            offset = file_offsets[line_number];
            line_length = line_lengths[line_number];
            char line[line_length];
            int read_line_check = INIT_CHECK;

            read_line_check = readLineFromFile(file_descriptor, line_length, offset, line);

            if (read_line_check == READ_LINE_ERROR)
            {
                printf("%s", "Can't read line");
                return GET_LINE_ERROR;
            }

            int print_line_check = INIT_CHECK;
            print_line_check = printLine(line, line_length);
            if (print_line_check == PRINT_LINE_ERROR)
            {
                printf("%s", "Can't print line");
                return GET_LINE_ERROR;
            }
        }
    }    
    return SUCCESS;
}

 int main(int argc, char* argv[])
 {
    off_t file_offsets[TABLE_SIZE]  = {0};
    size_t line_lengths[TABLE_SIZE]  = {0};

    int file_descriptor = INIT_CHECK;
    
    int number_of_lines = INIT_CHECK;

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

    number_of_lines = fillTable(file_descriptor, line_lengths, file_offsets);
    
    if (number_of_lines == FILL_TABLE_ERROR)
    {
        printf("Error with filling the table");
        return FAIL;
    }

    int get_lines_check = INIT_CHECK;
    
    get_lines_check = getLines(file_descriptor, line_lengths, file_offsets, number_of_lines);
    
    if (get_lines_check == GET_LINE_ERROR)
    {
        printf("Error with printing lines");
        return FAIL;
    }

    int close_check = INIT_CHECK;
    close_check = close(file_descriptor);

    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file");
        return FAIL;
    }

    return SUCCESS;
 }
