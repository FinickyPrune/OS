#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define OPEN_ERROR -1
#define READ_ERROR -1
#define CLOSE_ERROR -1
#define LSEEK_ERROR -1
#define ALLOC_ERROR NULL
#define STOP_NUMBER 0
#define TABLE_SIZE 256

int fillTable(int file_descriptor, size_t* line_lengths, size_t* file_offsets)
{
    char read_buffer[BUFSIZ];
    int actual_buffer_size =  0;

    actual_buffer_size = read(file_descriptor, read_buffer, BUFSIZ);
    
    if (actual_buffer_size == READ_ERROR)
    {
        perror("Can't read current text.");
        exit(EXIT_FAILURE);
    }

    int current_line_index = 1;
    int file_offset_index = 1;
    int current_offset = 0;
    
    while (actual_buffer_size > 0)
    {
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
        actual_buffer_size = read(file_descriptor, read_buffer, BUFSIZ);
        if (actual_buffer_size == READ_ERROR)
        {
            perror("Can't read current text.");
            exit(EXIT_FAILURE);
        }
    }

    return (current_line_index-1);
}

void getLines(int file_descriptor, size_t* line_lengths, size_t* file_offsets, int number_of_lines)
{
    char* write_buffer = NULL;
    size_t line_number = 0;

    while(printf("Enter number of line: ") &&  scanf("%d", &line_number))
    {
        
        if (line_number == STOP_NUMBER)
        {
            break;
        }
        if (line_number > number_of_lines || line_number < 0)
        {
            printf("%s", "Invalid number of line. Try again.\n");
        }
        else
        {
            write_buffer = (char*)realloc(write_buffer,sizeof(char) * line_lengths[line_number]);

            if (write_buffer == ALLOC_ERROR)
            {
                perror("Can't allocate memory with realloc.");
                exit(EXIT_FAILURE);
            }

            off_t lseek_check  = 0;
            lseek_check = lseek(file_descriptor, file_offsets[line_number], SEEK_SET);

            if ((int)lseek_check == LSEEK_ERROR)
            {
                perror("Seek error.");
                exit(EXIT_FAILURE);
            }
        
            int read_check = 0;
            read_check = read(file_descriptor, write_buffer, line_lengths[line_number]);

            if (read_check == READ_ERROR)
            {
                perror("Can't read current text.");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < line_lengths[line_number]; i++)
            {
                printf("%c", write_buffer[i]);
            }
            printf("\n");  
        }
        
    }

    free(write_buffer);
}

 int main(int argc, char* argv[])
 {
    size_t file_offsets[TABLE_SIZE]  = {0};
    size_t line_lengths[TABLE_SIZE]  = {0};

    size_t file_descriptor = 0;
    
    size_t number_of_lines = 0;

    file_descriptor = open(argv[1], O_RDONLY);

    if (file_descriptor == OPEN_ERROR)
    {
        perror("Can't open file.");
        return 0;
    }

    number_of_lines = fillTable(file_descriptor, line_lengths, file_offsets);
    
    getLines(file_descriptor, line_lengths, file_offsets, number_of_lines);
    
    int close_check = 0;
    close_check = close(file_descriptor);

    if (close_check == CLOSE_ERROR)
    {
        perror("Error with closing the file.");
        exit(EXIT_FAILURE);
    }

 
    return 0;
 }
