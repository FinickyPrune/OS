#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define TRUE 1
#define OPEN_ERROR -1
#define READ_ERROR -1
#define WRITE_ERROR -1
#define CLOSE_ERROR -1
#define LSEEK_ERROR -1
#define GET_LINE_ERROR -1
#define FILL_TABLE_ERROR -1
#define ALLOC_ERROR NULL
#define STOP_NUMBER 0
#define TABLE_SIZE 256
#define CONSOLE_INPUT_SIZE 100

int fillTable(int file_descriptor, size_t* line_lengths, size_t* file_offsets)
{
    char read_buffer[BUFSIZ];
    int actual_buffer_size =  1;

    int current_line_index = 1;
    int file_offset_index = 1;
    int current_offset = 0;
    
    while (actual_buffer_size > 0)
    {
        actual_buffer_size = read(file_descriptor, read_buffer, BUFSIZ);
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
                file_offsets[file_offset_index] = current_offset - line_lengths[current_line_index];
                current_line_index++;
                file_offset_index++;
            }
        }
    }

    return (current_line_index);


}

int getLines(int file_descriptor, size_t* line_lengths, size_t* file_offsets, int number_of_lines)
{
    char* line = NULL;
    char console_input[CONSOLE_INPUT_SIZE]; 
    size_t line_number = 0;
    char text_for_user[23] = "Enter number of line: ";


    while(TRUE)
    {
        int write_check = 0;
        write_check = write(STDOUT_FILENO, text_for_user, 23);
        if (write_check == WRITE_ERROR)
        {
            perror("Can't print message for user");
            return GET_LINE_ERROR;
        }
        
        int read_check = 0;
        read_check = read(STDIN_FILENO, console_input, CONSOLE_INPUT_SIZE);
        if (read_check == READ_ERROR)
        {
            perror("Can't read current text");
            return GET_LINE_ERROR;
        }

        char* endptr = NULL;
        line_number = strtoll(console_input, &endptr, 0);
        if (line_number == LONG_MAX || line_number == LONG_MIN)
        {
            perror("Invalid line number");
        }
        
        if (line_number == STOP_NUMBER)
        {
            break;
        }
        if (line_number > number_of_lines || line_number < 0)
        {
            printf("%s", "Invalid number of line. Try again.\n");
            continue;
        }
        if (line_lengths[line_number] != 0)
        {
            line = (char*)realloc(line,sizeof(char) * line_lengths[line_number]);

            if (line == ALLOC_ERROR)
            {
                perror("Can't allocate memory with realloc");
                free(line);
                return GET_LINE_ERROR;
            }

            off_t lseek_check  = 0;
            lseek_check = lseek(file_descriptor, file_offsets[line_number], SEEK_SET);

            if ((int)lseek_check == LSEEK_ERROR)
            {
                perror("Seek error");
                free(line);
                return GET_LINE_ERROR;
            }
        
            int read_check = 0;
            read_check = read(file_descriptor, line, line_lengths[line_number]);

            if (read_check == READ_ERROR)
            {
                perror("Can't read current text");
                free(line);
                return GET_LINE_ERROR;
            }

            write(STDOUT_FILENO, line, line_lengths[line_number]);
        }
        printf("\n");  
    }

    free(line);
    return 0;
}

 int main(int argc, char* argv[])
 {
    size_t file_offsets[TABLE_SIZE]  = {0};
    size_t line_lengths[TABLE_SIZE]  = {0};

    size_t file_descriptor = -1;
    
    size_t number_of_lines = -1;

    if (argc != 2)
    {
        printf("Usage: a.out f1\n");
        exit(EXIT_FAILURE);
    }
    
    file_descriptor = open(argv[1], O_RDONLY);

    if (file_descriptor == OPEN_ERROR)
    {
        perror("Can't open file");
        exit(EXIT_FAILURE);
    }

    number_of_lines = fillTable(file_descriptor, line_lengths, file_offsets);
    
    if (number_of_lines == FILL_TABLE_ERROR)
    {
        printf("Error with filling the table");
        exit(EXIT_FAILURE);
    }

    int get_lines_check = -1;
    
    get_lines_check = getLines(file_descriptor, line_lengths, file_offsets, number_of_lines);
    
    if (get_lines_check == GET_LINE_ERROR)
    {
        printf("Error with printing lines");
        exit(EXIT_FAILURE);
    }

    int close_check = 1;
    close_check = close(file_descriptor);

    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file");
        exit(EXIT_FAILURE);
    }

    return 0;
 }
