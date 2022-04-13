#include "time_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TimeStep *initialise_time_step(void)
{
	TimeStep *timeStep;

	if(!(timeStep = (TimeStep *) malloc(sizeof(TimeStep)))) {
		fprintf(stderr, "error: unable to initialise node.\n");
		exit(EXIT_FAILURE);
	}
	timeStep->next = timeStep->prev = NULL;
	timeStep->gates = NULL;

	return timeStep;
}

void free_time_step(TimeStep *timeStep)
{
	if(!timeStep)
		return;
	free(timeStep);
}

TimeList *initialise_time_list(void)
{
	TimeList *list;

	if(!(list = (TimeList *) malloc(sizeof(TimeList)))) {
		fprintf(stderr, "error: unable to initialise linked list.\n");
		exit(EXIT_FAILURE);
	}
	list->first = list->last = NULL;

	return list;
}

void free_time_list(TimeList *list)
{
	TimeStep *next;

	while(list->first) {
		next = list->first->next;
		free_time_step(list->first);
		list->first = next;
	}
	free(list);
}

void append_time_step(TimeList *list, Gate **gates)
{
	TimeStep *node;

	node = initialise_time_step();

	node->gates = gates;
	node->prev = list->last;
	if(list->last) {
		list->last->next = node;
	}
	list->last = node;
	if(!list->first)
		list->first = node;
}

void prepend_time_step(TimeList *list, Gate **gates)
{
	TimeStep *node;

	node = initialise_time_step();

	node->gates = gates;
	node->next = list->first;
	if(list->first) {
		list->first->prev = node;
	}
	list->first = node;
	if(!list->last)
		list->last = node;
}

void remove_first_time_step(TimeList *list)
{
	TimeStep *first;

	if(!list->first)
		return;
	first = list->first->next;
	free(list->first);
	list->first = first;
	if(list->first)
		list->first->prev = NULL;
	else
		list->last = NULL;

}

void remove_last_time_step(TimeList *list)
{
	TimeStep *last;

	if(!list->last)
		return;
	last = list->last->prev;
	free_time_step(list->last);
	list->last = last;
	if(list->last)
		list->last->next = NULL;
	else
		list->first = NULL;
}
