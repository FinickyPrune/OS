#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node 
{
	char* str_value;
	struct node* next;
} node;

typedef struct list
{
	node* head_node;
	node* last_node;
} list;

node* initHead();

list* initList();

void freeNode(node* currentNode);

node* initNode(char* new_string);

void pushNode(list* list, node* new_node);

void deleteList(list* list);