#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
    Simulation *simulation = set_up_simulation();
    initialise_qubits(4, simulation);

    apply_gate(0, (float *) hadamard, simulation);
    apply_gate(1, (float *) not, simulation);
    apply_gate(3, (float *) not, simulation);
    apply_gate(1, (float *) hadamard, simulation);
    apply_controlled_gate(2, 1, (float *) not, simulation);
    apply_controlled_gate(1, 0, (float *) not, simulation);
    apply_gate(1, (float *) not, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);
 
}
