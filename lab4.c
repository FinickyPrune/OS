#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ALLOC_ERROR NULL

typedef struct node 
{
	char* str_value;
	struct node* next;
} node;

node* initNode() 
{
	node* head = NULL;
	int node_size = sizeof(node);
	head = (node*)malloc(node_size);
	
	if (head == ALLOC_ERROR)
	{
		perror("Can't allocate memory for head node.");
		exit(EXIT_FAILURE);
	}
	
	head->next = NULL;
	head->str_value = NULL;
	return head;
}

void freeNode(node* currentNode) 
{
	free(currentNode->str_value);
	free(currentNode);
}

node* addNode(char* new_string) 
{
	node* new_node = NULL;
	int node_size = sizeof(node);
	new_node = (node*)malloc(node_size);

	if (new_node == ALLOC_ERROR)
	{
		perror("Can't allocate memory for new node.");
		exit(EXIT_FAILURE);
	}

	int string_length = strlen(new_string);

	// size_t strlen(char*) возвращает длину строки, НЕ считая нулевой символ
		
	new_node->str_value = (char*)malloc(string_length + 1);
	char* string_ptr = new_node->str_value;

	if (string_ptr == ALLOC_ERROR)
	{
		perror("Can't allocate memory for new line.");
		exit(EXIT_FAILURE);
	}

	strcpy(string_ptr, new_string);

	// char* strcpy(char* s1, char* s2) копирует содержимое s2 в s1, в том числе нулевой символ, возвращает указатель на s1

	new_node->next = NULL;
	return new_node;

}

void deleteList(node* head)
{
	node* i = head;
	node* j = NULL;
	while (i != NULL) 
	{
		j = i->next;
		freeNode(i);
		i = j;
	}
}

void getLines(node* head, char* line)
{
	printf("%s", "Enter your lines:\n");
	
	node* currentNode = head;
	char* fgets_check;
	
	while ((fgets_check = fgets(line, BUFSIZ + 1, stdin)) != NULL)
	{
		// extern FILE* stdin - стандартный поток ввода.
		// char* fgets(char* s, int n, FILE* stream) считывает из stream в s, пока не считает n-1 символ или не повстречает символ начала строки. Затем к строке добавляется нулевой символ

		if (line[0] == '.')
		{
			break;
		}
		currentNode->next = addNode(line);
		currentNode = currentNode->next;
	}

	if (fgets_check == NULL)
	{
		perror("Can't read the line.");
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
	char line[BUFSIZ];
	node* head = NULL;

	head = initNode();
	
	getLines(head, line);
	
	printLines(head);
	
	deleteList(head);

	return 0;
}