#include "StringList.h"

#define STOP_SYMBOL '.'
#define LIST_ERROR NULL
#define GET_LINES_ERROR -1
#define PRINT_LINES_ERROR -1

int getLines(list* list)
{
	printf("%s", "Enter your lines:\n");
	
    char line[BUFSIZ];
	node* currentNode = NULL;
	char* fgets_check;
	
	while ((fgets_check = fgets(line, BUFSIZ + 1, stdin)) != NULL)
	{
		// extern FILE* stdin - стандартный поток ввода.
		// char* fgets(char* s, int n, FILE* stream) считывает из stream в s, пока не считает n-1 символ или не повстречает символ начала строки. 
		// Затем к строке добавляется нулевой символ

		if (line[0] == STOP_SYMBOL)
		{
			break;
		}

        currentNode = initNode(line);

        if (currentNode == LIST_ERROR)
        {
            printf("%s", "Can't init new node.");
		    deleteList(list);
            return GET_LINES_ERROR;
        }
            
        pushNode(list, currentNode);
	}

	if (fgets_check == NULL)
	{
		perror("Can't read the line.");
		deleteList(list);
        return GET_LINES_ERROR;
	}

	return 0;
}

int printLines(list* list)
{
	printf("%s", "\nYour lines:\n");
	
	int fputs_check;
	node* i = NULL;

	for (i = list->head_node->next; i != NULL; i = i->next)
	{
		fputs_check = fputs(i->str_value, stdout);

		// extern FILE* stdout - стандартный поток вывода. 
		// int fputs(char* s, FILE* stream) - пишет s в stream.

		if (fputs_check == EOF)
		{
			perror("Can't print this string.");
			deleteList(list);
			return PRINT_LINES_ERROR;
		}
	}

	return -1;
}

int main() 
{
	list* list = NULL;

	list = initList();

	if (list == LIST_ERROR)
    {
        printf("%s", "Can't init list.");
        return -1;
    }

	int check = 0;
	check = getLines(list);
	
	if (check == GET_LINES_ERROR)
	{
		return -1;
	}
	
	check = printLines(list);

	if (check == PRINT_LINES_ERROR)
	{
		return -1;
	}
	
	deleteList(list);

	return 0;
}