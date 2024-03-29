#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void assert(int status)
{
    if(status == 1)
        return;

    printf("\033[1;31mFailed\n \033[0m");
    exit(EXIT_FAILURE);
}

void test_initialise_circuit()
{
    printf("Testing initialise_circuit: ");

    Circuit *circuit = initialise_circuit(2);

    // test circuit
    assert(circuit->num_qubits == 2);
    assert(circuit->steps != NULL);

    free_circuit(circuit);

    printf("Pass\n");
}

void test_initialise_gate()
{
    printf("Testing initialise_gate: ");

    Gate *gate = initialise_gate(X, 0, 1, true);

    // test gate
    assert(gate->type == X);
    assert(gate->target == 0);
    assert(gate->control == 1);
    assert(gate->isControlled);

    free(gate);

    printf("Pass\n");
}

void test_add_time_step()
{
    printf("Testing add_time_step: ");

    Circuit *circuit = initialise_circuit(1);
    add_time_step(circuit);

    // test time step has been added
    assert(circuit->steps->first != NULL);
    assert(circuit->steps->first->gates != NULL);

    free_circuit(circuit);

    printf("Pass\n");
}

void test_add_gate()
{
    printf("Testing add_gate: ");

    Circuit *circuit = initialise_circuit(1);
    add_gate(HADAMARD, 0, circuit);

    // test gate has been added to circuit
    Gate *gate = circuit->steps->last->gates[0];
    assert(gate->type == HADAMARD);
    assert(gate->target == 0);
    assert(!gate->isControlled);

    free_circuit(circuit);

    printf("Pass\n");
}

void test_add_controlled_gate()
{
    printf("Testting add_controlled_gate: ");

    Circuit *circuit = initialise_circuit(2);
    add_controlled_gate(X, 0, 1, circuit);

    // test gate has been added to circuit
    Gate *gate = circuit->steps->last->gates[0];
    assert(gate->type == X);
    assert(gate->target == 0);
    assert(gate->control == 1);
    assert(gate->isControlled);

    free_circuit(circuit);

    printf("Pass\n");
}

int main()
{
    printf("\033[1;32m");

    test_initialise_circuit();
    test_initialise_gate();
    test_add_time_step();
    test_add_gate();
    test_add_controlled_gate();
    
    printf("\033[0m");
}
