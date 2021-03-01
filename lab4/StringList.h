#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node 
{
	char* str_value;
	struct node* next;
} node;

node* initHead();

void freeNode(node* currentNode);

node* initNode(char* new_string);

void pushNode(node* head, node* newNode);

void deleteList(node* head);