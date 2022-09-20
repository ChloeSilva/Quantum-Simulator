#ifndef _CIRCUIT_H
#define _CIRCUIT_H

#include "time_list.h"
#include <stdbool.h>

typedef struct Circuit
{
    int num_qubits;
    int num_gates;
    TimeList *steps;
} Circuit;

Circuit *initialise_circuit(int);
Gate *initialise_gate(GateType, int, int, bool);
Gate *get_gate(void *, int);
void free_circuit(Circuit *);
void add_time_step(Circuit *);
void add_gate(GateType, int, Circuit *);
void add_controlled_gate(GateType, int, int, Circuit *);

#endif