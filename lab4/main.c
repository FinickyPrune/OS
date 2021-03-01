#include "StringList.h"

#define STOP_SYMBOL '.'
#define LIST_ERROR NULL

void getLines(node* head)
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
		    deleteList(head);
            exit(EXIT_FAILURE);
        }
            
        pushNode(head, currentNode);
	}

	if (fgets_check == NULL)
	{
		perror("Can't read the line.");
		deleteList(head);
        exit(EXIT_FAILURE);
	}
}

void printLines(node* head)
{
	printf("%s", "\nYour lines:\n");
	
	int fputs_check;
	node* i = NULL;

	for (i = head->next; i != NULL; i = i->next)
	{
		fputs_check = fputs(i->str_value, stdout);

		// extern FILE* stdout - стандартный поток вывода. 
		// int fputs(char* s, FILE* stream) - пишет s в stream.

		if (fputs_check == EOF)
		{
			perror("Can't print this string.");
		}
	}
}

int main() 
{
	node* head = NULL;

	head = initHead();
	
    if (head == LIST_ERROR)
    {
        printf("%s", "Can't init head.");
        return -1;
    }

	getLines(head);
	
	printLines(head);
	
	deleteList(head);

	return 0;
}