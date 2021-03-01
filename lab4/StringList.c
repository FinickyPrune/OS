#include "StringList.h"

#define ALLOC_ERROR NULL

node* initHead() 
{
	node* head = NULL;
	int node_size = sizeof(node);
	head = (node*)malloc(node_size);
	
	if (head == ALLOC_ERROR)
	{
		perror("Can't allocate memory for head node.");
		return NULL;
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

node* initNode(char* new_string) 
{
	node* new_node = NULL;
	int node_size = sizeof(node);
	new_node = (node*)malloc(node_size);

	if (new_node == ALLOC_ERROR)
	{
		perror("Can't allocate memory for new node.");
		return NULL;
	}

	int string_length = -1;
	string_length = strlen(new_string);

	if (string_length < 0)
	{
		printf("Error with counting string length.");
		return NULL;
	}	

	// size_t strlen(char*) возвращает длину строки, НЕ считая нулевой символ
		
	new_node->str_value = (char*)malloc(string_length + 1);
	char* string_ptr = new_node->str_value;

	if (string_ptr == ALLOC_ERROR)
	{
		perror("Can't allocate memory for new line.");
		return NULL;
	}

	char* cpy_check = NULL;

	cpy_check = strncpy(string_ptr, new_string, string_length);

	if (cpy_check == NULL)
	{
		printf("Error with coping the string.");
		return NULL;
	}

	// char* strcpy(char* s1, char* s2) копирует содержимое s2 в s1, в том числе нулевой символ, возвращает указатель на s1

	new_node->next = NULL;
	return new_node;

}

void pushNode(node* head, node* new_node)
{
	node* curr_node = head;

	while (curr_node->next != NULL)
	{
		curr_node = curr_node->next;
	}

	curr_node->next = new_node;
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

