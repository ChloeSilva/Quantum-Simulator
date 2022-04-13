#include "linked_list.h"

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
	timeStep->data = NULL;

	return timeStep;
}

void free_time_step(TimeStep *timeStep)
{
	if(!timeStep)
		return;
	free(timeStep);
}

LinkedList *initialise_linked_list(void)
{
	LinkedList *list;

	if(!(list = (LinkedList *) malloc(sizeof(LinkedList)))) {
		fprintf(stderr, "error: unable to initialise linked list.\n");
		exit(EXIT_FAILURE);
	}
	list->head = list->tail = NULL;

	return list;
}

void free_linked_list(LinkedList *list)
{
	TimeStep *next;

	while(list->head) {
		next = list->head->next;
		free_time_step(list->head);
		list->head = next;
	}
	free(list);
}

void append_linked_list(LinkedList *list, Gate **data)
{
	TimeStep *node;

	node = initialise_time_step();

	node->data = data;
	node->prev = list->tail;
	if(list->tail) {
		list->tail->next = node;
	}
	list->tail = node;
	if(!list->head)
		list->head = node;
}

void prepend_linked_list(LinkedList *list, Gate **data)
{
	TimeStep *node;

	node = initialise_time_step();

	node->data = data;
	node->next = list->head;
	if(list->head) {
		list->head->prev = node;
	}
	list->head = node;
	if(!list->tail)
		list->tail = node;
}

void remove_head_linked_list(LinkedList *list)
{
	TimeStep *head;

	if(!list->head)
		return;
	head = list->head->next;
	free(list->head);
	list->head = head;
	if(list->head)
		list->head->prev = NULL;
	else
		list->tail = NULL;

}

void remove_tail_linked_list(LinkedList *list)
{
	TimeStep *tail;

	if(!list->tail)
		return;
	tail = list->tail->prev;
	free_time_step(list->tail);
	list->tail = tail;
	if(list->tail)
		list->tail->next = NULL;
	else
		list->head = NULL;
}
