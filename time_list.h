#ifndef _TIME_LIST_H
#define _TIME_LIST_H

#include <stdbool.h>

typedef enum {HADAMARD, X, Y, Z, CONTROL, NONE} GateType;

typedef struct Gate
{
    GateType type;
    int target;
    int control;
    bool isControlled;
} Gate;

typedef struct TimeStep
{
	Gate **gates;
	struct TimeStep *next;
	struct TimeStep *prev;
} TimeStep;

typedef struct TimeList
{
	TimeStep *first;
	TimeStep *last;
} TimeList;

TimeStep *initialise_time_step(void);
void free_time_step(TimeStep *);
TimeList *initialise_time_list(void);
void free_time_list(TimeList *);
void append_time_step(TimeList *, Gate **);
void prepend_time_step(TimeList *, Gate **);
void remove_first_time_step(TimeList *, int);
void remove_last_time_step(TimeList *, int);

#endif
