#include "shor.h"

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

void test_add(int a, int b)
{
    int binary[QUBITS];
    int_to_binary(b, binary);
    reverse(binary);

    Simulation *simulation = set_up_simulation();
    initialise_qubits(QUBITS, simulation);

    for(int i=0; i<QUBITS; i++)
        if(binary[i] == 1)
            apply_gate(i, x, simulation);

    apply_qft(QUBITS, simulation);
    apply_add(a, simulation);
    apply_inv_qft(QUBITS, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);
}

void test_sub(int a, int b)
{
    int binary[QUBITS];
    int_to_binary(b, binary);
    reverse(binary);

    Simulation *simulation = set_up_simulation();
    initialise_qubits(QUBITS, simulation);

    for(int i=0; i<QUBITS; i++)
        if(binary[i] == 1)
            apply_gate(i, x, simulation);

    apply_qft(QUBITS, simulation);
    apply_inv_add(a, simulation);
    apply_inv_qft(QUBITS, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);
}

void test_add_a_mod_n(int a, int b, int c1, int c2, int zero, int n)
{
    int binary[QUBITS];
    int_to_binary(b, binary);
    reverse(binary);

    Simulation *simulation = set_up_simulation();
    initialise_qubits(QUBITS+3, simulation);

    apply_gate(c1, x, simulation);
    apply_gate(c2, x, simulation);
    
    for(int i=0; i<QUBITS; i++)
        if(binary[i] == 1)
            apply_gate(i, x, simulation);

    apply_qft(QUBITS, simulation);

    add_a_mod_n(a, n, c1, c2, zero, simulation);

    apply_inv_qft(QUBITS, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);
}

void test_mult_a_mod_n_c(int a, int b, int d, int n, int c, int zero)
{
    int binary_b[QUBITS];
    int binary_d[QUBITS];
    
    int_to_binary(b, binary_b);
    int_to_binary(d, binary_d);
    reverse(binary_b);
    reverse(binary_d);

    Simulation *simulation = set_up_simulation();
    initialise_qubits(QUBITS*2+2, simulation);

    apply_gate(c, x, simulation);

    for(int i=0; i<QUBITS; i++)
        if(binary_b[i] == 1)
            apply_gate(i, x, simulation);

    for(int i=0; i<QUBITS-1; i++)
        if(binary_d[i] == 1)
            apply_gate(QUBITS+i, x, simulation);

    apply_qft(QUBITS, simulation);

    mult_a_mod_n_c(a, n, c, zero, simulation);

    apply_inv_qft(QUBITS, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);
}

int main()
{
    printf("\033[1;32m");

    test_add();
    test_sub();
    test_add_a_mod_n();
    test_mult_a_mod_n_c();
    
    printf("\033[0m");
}
