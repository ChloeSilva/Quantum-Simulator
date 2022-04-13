#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdbool.h>

typedef enum {HADAMARD, NOT, CONTROL, NONE} Gate_Type;

typedef struct Gate
{
    Gate_Type type;
    int target;
    int control;
    bool isControlled;
} Gate;

typedef struct TimeStep
{
	Gate **data;
	struct TimeStep *next;
	struct TimeStep *prev;
} TimeStep;

typedef struct LinkedList
{
	TimeStep *head;
	TimeStep *tail;
} LinkedList;

TimeStep *initialise_time_step(void);
void free_time_step(TimeStep *);
LinkedList *initialise_linked_list(void);
void free_linked_list(LinkedList *);
void append_linked_list(LinkedList *, Gate **);
void prepend_linked_list(LinkedList *, Gate **);
void remove_head_linked_list(LinkedList *);
void remove_tail_linked_list(LinkedList *);

#endif
