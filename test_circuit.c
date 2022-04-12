#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_initialise_circuit()
{
    printf("Testing initialise_circuit: ");

    Circuit *circuit = initialise_circuit(2);

    // test circuit
    assert(circuit->num_qubits == 2);
    assert(circuit->list != NULL);

    printf("Pass\n");
}

int main()
{
    test_initialise_circuit();
}
