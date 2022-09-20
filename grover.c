#include "simulation.h"
#include "circuit_synthesis.h"

#include <stdio.h>
#include <math.h>

#define QUBITS 12
#define iterations 1

void reverse_2(int *list)
{
    int temp;

    for(int i=0; i<QUBITS/2; i++) {
        temp = list[i];
        list[i] = list[QUBITS-1-i];
        list[QUBITS-1-i] = temp;
    }

}

void int_to_binary(int n, int *binary_list)
{
    int size = QUBITS-1;
    for(int i=size; i>=0; i--) {
        if(n >= (int) pow((double) 2, (double) i)) {
            binary_list[i] = 1;
            n -= (int) pow((double) 2, (double) i);
        } else {
            binary_list[i] = 0;
        }
    }
}

int request_user_input()
{
    int input, error;
    char c = '\0';

    printf("Please enter a number between 0-%d to search for: ", (int) pow(2, QUBITS)-1);
    error = scanf("%d",&input);
    
    if (error == EOF || error == 0 || input  < 0 || input > pow(2, QUBITS)-1) 
    {
        printf("\nIncorrect input: ");
        while(c != '\n')
            c = getchar();
        return request_user_input();
    } else {
        return input;
    }
}

void apply_control_z(Simulation *simulation)
{
    int *circuit = synthesise_multi_cnot(QUBITS-1);
    int i = 0;
    int n = pow(2, (QUBITS-2));

    float root_x[8] = {0.5*(1+cosf(M_PI/n)), 0.5*sin(M_PI/n), 0.5*(1-cosf(M_PI/n)), -0.5*sin(M_PI/n),
                       0.5*(1-cosf(M_PI/n)), -0.5*sin(M_PI/n), 0.5*(1+cosf(M_PI/n)), 0.5*sin(M_PI/n)};

    float inv_root_x[8] = {0.5*(1+cosf(M_PI/n)), -0.5*sin(M_PI/n), 0.5*(1-cosf(M_PI/n)), 0.5*sin(M_PI/n),
                           0.5*(1-cosf(M_PI/n)), 0.5*sin(M_PI/n), 0.5*(1+cosf(M_PI/n)), -0.5*sin(M_PI/n)};

    apply_gate(QUBITS-1, (float *) hadamard, simulation);

    while(circuit[i] != QUBITS) {
        int control = circuit[i++];
        int target = circuit[i++];
        int parity = circuit[i++];

        if(target != QUBITS-1) {
            apply_controlled_gate(target, control, x, simulation);
        } else {
            if(parity == 0) {
                apply_controlled_gate(target, control, root_x, simulation);
            } else {
                apply_controlled_gate(target, control, inv_root_x, simulation);
            }
        }
    }

    apply_gate(QUBITS-1, (float *) hadamard, simulation);

    free(circuit);
}

void apply_hadamard(Simulation *simulation)
{
    for(int i=0; i<QUBITS; i++) {
        apply_gate(i, (float *) hadamard, simulation);
    }
}

void apply_x(Simulation *simulation)
{
    for(int i=0; i<QUBITS; i++) {
        apply_gate(i, (float *) x, simulation);
    }
}

void apply_diffuser(Simulation *simulation)
{
    apply_hadamard(simulation);
    apply_x(simulation);
    apply_control_z(simulation);
    apply_x(simulation);
    apply_hadamard(simulation);
}

void apply_oracle(int n, Simulation *simulation)
{
    int binary_list[QUBITS];
    int_to_binary(n, binary_list);
    reverse_2(binary_list);

    for(int i=0; i<QUBITS; i++)
        if(binary_list[i]) {
            apply_gate(i, (float *) x, simulation);
        }
    
    apply_control_z(simulation);

    for(int i=0; i<QUBITS; i++)
        if(!binary_list[i]) {
            apply_gate(i, (float *) x, simulation);
        }
}

void run_grover(int n, Simulation *simulation)
{
    simulation->epsilon = 0.001;
    initialise_qubits(QUBITS, simulation);
    apply_hadamard(simulation);
    for(int i=0; i<iterations; i++) {
        apply_oracle(n, simulation);
        apply_diffuser(simulation);
    }
}

int main()
{
    int input = request_user_input();
    Simulation *simulation = set_up_simulation();
    run_grover(input, simulation);
    
    measure(simulation);
    printf("most significant amplitude: ");
    print_results(simulation);
    deallocate_resources(simulation);
}
