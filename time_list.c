#include "time_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initialises new time step for circuit.
 * Allocates memory and sets default values for its members.
 * 
 * @return pointerr to new time step
 */
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

/**
 * @brief Frees a time step.
 * 
 * @param timeStep the time step to be freed
 */
void free_time_step(TimeStep *timeStep)
{
	if(!timeStep)
		return;
	free(timeStep);
}

/**
 * @brief Initialises a time list.
 * Allocates memory and sets default values for its members.
 * 
 * @return pointer to new time list
 */
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

/**
 * @brief Frees time list and all its time steps.
 * 
 * @param list to be freed.
 */
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

/**
 * @brief Appends time step to the list
 * 
 * @param list The list to append to
 * @param gates The gates at the time of the time slot
 */
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

/**
 * @brief prepends time step to the list
 * 
 * @param list The list to prepend to
 * @param gates The gates at the time of the time slot
 */
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

/**
 * @brief Removes the first time step of the list and frees all
 * unnecessary resources.
 * 
 * @param list The list of gates in the time list
 * @param num_gates The number of gates in each time slot
 */
void remove_first_time_step(TimeList *list, int num_gates)
{
	TimeStep *first;

	if(!list->first)
		return;
	first = list->first->next;
	
	for(int i=0; i<num_gates; i++)
            free(list->first->gates[i]);
        free(list->first->gates);
	free(list->first);
	
	list->first = first;
	if(list->first)
		list->first->prev = NULL;
	else
		list->last = NULL;

}

/**
 * @brief Removes the last time step of the list and freees all
 * unnecessary resources.
 * 
 * @param list Thee list of gates in the time list
 * @param num_gates The number of gates in each timee slot
 */
void remove_last_time_step(TimeList *list, int num_gates)
{
	TimeStep *last;

	if(!list->last)
		return;
	last = list->last->prev;
		
	for(int i=0; i<num_gates; i++)
            free(list->last->gates[i]);
        free(list->last->gates);
	free(list->last);
	
	list->last = last;
	if(list->last)
		list->last->next = NULL;
	else
		list->first = NULL;
}
