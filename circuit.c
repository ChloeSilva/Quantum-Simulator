#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>

Circuit *initialise_circuit(int size)
{
    Circuit *circuit = (Circuit *) malloc(sizeof(Circuit));
    if(!circuit) {
        fprintf(stderr, "error: unable to initialise Circuit.\n");
        exit(EXIT_FAILURE);
    }

    circuit->num_qubits = size;
    circuit->list = initialise_linked_list();

    return circuit;
}

Gate *initialise_gate(Gate_Type type, bool isControlled, int control)
{
    Gate *gate = (Gate *) malloc(sizeof(Gate));
    if(!gate) {
        fprintf(stderr, "error: unable to initialise gate.\n");
        exit(EXIT_FAILURE);
    }

    gate->type = type;
    gate->isControlled = isControlled;
    gate->control = control;

    return gate;
}
