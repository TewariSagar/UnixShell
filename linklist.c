#include "global.h"

struct node* createAliasList()
{
	struct node* tail = malloc(sizeof(node)); 
	if( tail != NULL)
	{
		tail -> next = NULL;
		tail -> data = NULL;
		tail -> name = NULL;
		return tail;
	}
}

void aliasInsert(struct node **head, char *data, char *name)
{
	struct node* new = malloc(sizeof(node));
	new->data = data;
	new->name = name;
	new->next = *head;
	*head = new;
}

struct node* aliasGet(struct node *head, char *name)
{
	struct node* currNode = head;
	while(currNode->next != NULL && strcmp(name, (currNode->name)))
	{
		currNode = currNode->next;
	}
	if(currNode->next != NULL)
	{
		return currNode;
	}
}

void aliasDelete(struct node **head, char *name)
{
	struct node* currNode = *head;
	struct node* prevNode = NULL;
	//printf("DEBUG aliasDelete: name %s\n", name);
	while(currNode->next != NULL && strcmp(name, (currNode->name)))
	{
		prevNode = currNode;
		currNode = currNode->next;
	}
	if(currNode->next != NULL && !strcmp(name, (currNode->name)))
	{
		if(currNode == *head)
		{
			*head = currNode->next;
			//printf("DEBUG aliasDelete: currNode == head\n");
		}

		if(prevNode != NULL)
			prevNode->next = currNode->next;
		currNode->next = NULL;
		free(currNode->name);
		free(currNode->data);
		free(currNode);
	}
}

void aliasPrint(struct node *head)
{
	struct node* currNode = head;
	while(currNode->next != NULL)
	{
		printf("%s \t " , currNode->name);
		printf("%s\n" , currNode->data);
		currNode = currNode->next;
	}
}

