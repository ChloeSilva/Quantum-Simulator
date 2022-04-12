#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

typedef struct Node
{
	void *data;
	struct Node *next;
	struct Node *prev;
} Node;

typedef struct LinkedList
{
	Node *head;
	Node *tail;
} LinkedList;

Node *initialise_node(void);
void free_node(Node *);
LinkedList *initialise_linked_list(void);
void free_linked_list(LinkedList *);
void append_linked_list(LinkedList *, void *);
void prepend_linked_list(LinkedList *, void *);
void remove_head_linked_list(LinkedList *);
void remove_tail_linked_list(LinkedList *);

#endif
