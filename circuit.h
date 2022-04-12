#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include "linked_list.h"
#include <stdbool.h>

typedef struct Circuit
{
    int num_qubits;
    LinkedList *steps;
} Circuit;

Circuit *initialise_circuit(int);
Gate *initialise_gate(Gate_Type, int, int, bool);
Gate *get_gate(void *, int);
void free_circuit(Circuit *);
void add_time_step(Circuit *);
void add_gate(Gate_Type, int, Circuit *);
void add_controlled_gate(Gate_Type, int, int, Circuit *);

#endif
