#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>

Circuit *initialise_circuit(int size)
{
    // initialise circuit
    Circuit *circuit = (Circuit *) malloc(sizeof(Circuit));
    if(!circuit) {
        fprintf(stderr, "error: unable to initialise Circuit.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    circuit->num_qubits = size;
    circuit->steps = initialise_time_list();

    add_time_step(circuit);

    return circuit;
}

Gate *initialise_gate(GateType type, int target, int control, bool isControlled)
{
    // initialise gate
    Gate *gate = (Gate *) malloc(sizeof(Gate));
    if(!gate) {
        fprintf(stderr, "error: unable to initialise gate.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    gate->type = type;
    gate->target = target;
    gate->control = control;
    gate->isControlled = isControlled;

    return gate;
}

void free_circuit(Circuit *circuit)
{
    TimeStep *next;

	while(circuit->steps->first) {
		next = circuit->steps->first->next;
        for(int i=0; i<circuit->num_qubits; i++)
            free(circuit->steps->first->gates[i]);
        free(circuit->steps->first->gates);
        free(circuit->steps->first);
		circuit->steps->first = next;
	}


    free_time_list(circuit->steps);
    free(circuit);
}

void add_time_step(Circuit *circuit)
{
    // allocate memory for gates at the new time step
    Gate **gate_slots = (Gate **) malloc(sizeof(Gate *)*circuit->num_qubits);
    if(!gate_slots) {
        fprintf(stderr, "error: unable to initialise gate slots.\n");
        exit(EXIT_FAILURE);
    }

    // initialise gate slots to be empty
    for(int i=0; i<circuit->num_qubits; i++)
        gate_slots[i] = initialise_gate(NONE, 0, 0, false);

    // adds new gate slots to the circuit
    append_time_step(circuit->steps, gate_slots);
}

void add_gate(GateType type, int target, Circuit *circuit)
{
    // check the target bit exits in the circuit
    if(target >= circuit->num_qubits) {
        fprintf(stderr, "error: target bit is not in circuit.\n");
        exit(EXIT_FAILURE);
    }

    Gate *gate = initialise_gate(type, target, 0, false);
    
    // check if target slot is full and create new time step if so
    Gate **gate_slot = circuit->steps->last->gates;
    if(gate_slot[target]->type != NONE)
        add_time_step(circuit);

    // add gate to circuit
    gate_slot = circuit->steps->last->gates;
    free(gate_slot[target]);
    gate_slot[target] = gate;
}

void add_controlled_gate(GateType type, int target, int control, Circuit *circuit)
{
    // check if target and control bits exit in the circuit
    if(target >= circuit->num_qubits) {
        fprintf(stderr, "error: target bit is not in circuit.\n");
        exit(EXIT_FAILURE);
    }

    if(control >= circuit->num_qubits) {
        fprintf(stderr, "error: target bit is not in circuit.\n");
        exit(EXIT_FAILURE);
    }

    //check if target and control bits are the same
    if(target == control) {
        fprintf(stderr, "error: target and control bits must be different.\n");
        exit(EXIT_FAILURE);
    }

    Gate *target_gate = initialise_gate(type, target, control, true);
    Gate *control_gate = initialise_gate(CONTROL, 0, 0, false);
    
    // check if target and control slots are full and create new time step if so
    Gate **gate_slot = circuit->steps->last->gates;
    if(gate_slot[target]->type != NONE 
        || gate_slot[control]->type != NONE)
        add_time_step(circuit);

    // add gate to circuit
    gate_slot = circuit->steps->last->gates;
    free(gate_slot[target]);
    free(gate_slot[control]);
    gate_slot[target] = target_gate;
    gate_slot[control] = control_gate;
}
