#ifndef linklist_h
#define linklist_h


struct node
{
	char* name;
	char* data;
	struct node* next;
} node;

struct node* createAliasList();
void aliasInsert(struct node **head, char *data, char *name);
struct node* aliasGet(struct node *head, char *name);
void aliasDelete(struct node **head, char *name);
void aliasPrint(struct node *head);
#endif

