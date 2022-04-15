#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main()
{
    printf("Testing simulation: ");

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
   
   assert(round(simulation->probabilities[0]*100) == 0);
   assert(round(simulation->probabilities[1]*100) == 0);
   assert(round(simulation->probabilities[2]*100) == 0);
   assert(round(simulation->probabilities[3]*100) == 0);
   assert(round(simulation->probabilities[4]*100) == 0);
   assert(round(simulation->probabilities[5]*100) == 0);
   assert(round(simulation->probabilities[6]*100) == 0);
   assert(round(simulation->probabilities[7]*100) == 0);
   assert(round(simulation->probabilities[8]*100) == 0);
   assert(round(simulation->probabilities[9]*100) == 25);
   assert(round(simulation->probabilities[10]*100) == 25);
   assert(round(simulation->probabilities[11]*100) == 0);
   assert(round(simulation->probabilities[12]*100) == 25);
   assert(round(simulation->probabilities[13]*100) == 0);
   assert(round(simulation->probabilities[14]*100) == 0);
   assert(round(simulation->probabilities[15]*100) == 25);

    deallocate_resources(simulation);

    printf("Pass\n");
}
