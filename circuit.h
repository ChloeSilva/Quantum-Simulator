#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include "linked_list.h"
#include <stdbool.h>

typedef enum {HADAMARD, NOT, NONE} Gate_Type;

typedef struct Circuit
{
    int num_qubits;
    LinkedList *list;
} Circuit;

typedef struct Gate
{
    Gate_Type type;
    bool isControlled;
    int control;
} Gate;

Circuit *initialise_circuit(int);
Gate *initialise_gate(Gate_Type, bool, int);
void add_gate(Gate *, int, Circuit);

#endif
