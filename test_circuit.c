#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

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

    Gate *gate = initialise_gate(NOT, 0, 1, true);

    // test gate
    assert(gate->type == NOT);
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
    assert(circuit->steps->head != NULL);
    assert(circuit->steps->head->data != NULL);

    free_circuit(circuit);

    printf("Pass\n");
}

void test_add_gate()
{
    printf("Testing add_gate: ");

    Circuit *circuit = initialise_circuit(1);
    add_gate(HADAMARD, 0, circuit);

    // test gate has been added to circuit
    Gate *gate = circuit->steps->tail->data[0];
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
    add_controlled_gate(NOT, 0, 1, circuit);

    // test gate has been added to circuit
    Gate *gate = circuit->steps->tail->data[0];
    assert(gate->type == NOT);
    assert(gate->target == 0);
    assert(gate->control == 1);
    assert(gate->isControlled);

    free_circuit(circuit);

    printf("Pass\n");
}

int main()
{
    test_initialise_circuit();
    test_initialise_gate();
    test_add_time_step();
    test_add_gate();
    test_add_controlled_gate();
}
