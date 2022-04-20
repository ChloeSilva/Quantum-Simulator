#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void assert(int status)
{
    if(status == 1)
        return;

    printf("\033[1;31mFailed\n \033[0m");
    exit(EXIT_FAILURE);
}

void test_toffoli()
{
    printf("Testing toffoli: ");

    // given
    Simulation *simulation = set_up_simulation();
    initialise_qubits(3, simulation);
    apply_gate(0, (float *) not, simulation);
    apply_gate(1, (float *) not, simulation);
    apply_controlled_controlled_gate(2, 1, 0, (float *) not, simulation);

    // when
    measure(simulation);

    // then
    assert(round(simulation->probabilities[0]*100) == 0);
    assert(round(simulation->probabilities[1]*100) == 0);
    assert(round(simulation->probabilities[2]*100) == 0);
    assert(round(simulation->probabilities[3]*100) == 0);
    assert(round(simulation->probabilities[4]*100) == 0);
    assert(round(simulation->probabilities[5]*100) == 0);
    assert(round(simulation->probabilities[6]*100) == 0);
    assert(round(simulation->probabilities[7]*100) == 100);

    printf("Pass\n");
}

void test_simulation()
{
    printf("Testing simulation: ");

    // given
    Simulation *simulation = set_up_simulation();
    initialise_qubits(4, simulation);
    apply_gate(0, (float *) hadamard, simulation);
    apply_gate(1, (float *) not, simulation);
    apply_gate(3, (float *) not, simulation);
    apply_gate(1, (float *) hadamard, simulation);
    apply_controlled_gate(2, 1, (float *) not, simulation);
    apply_controlled_gate(1, 0, (float *) not, simulation);
    apply_gate(1, (float *) not, simulation);
    
    // when
    measure(simulation);
   
    // then
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

int main()
{
    printf("\033[1;32m");

    test_toffoli();
    test_simulation();
    
    printf("\033[0m");
}
