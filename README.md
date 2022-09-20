/*! \mainpage Quantum Simulator

## Installation

As the quantum simulator is a library, simply download the folder and include "simulation.h" in the desired file. Make sure to link simulation and OpenCL during compilation.
Currently only implemented for mac.

To run unit and ensure it is working type, in the terminal:

    make run_all_tests



## API

The simulator provides the following functions:

Call to initialise a new simulation and store the pointer to call functions on the simulation.

    Simulation *simulation = Simulation *set_up_simulation();

Call to initialise qubits to |0...0> in simulation

    initialise_qubits(3, simulation);

Call to apply a single qubit gate

    apply_gate(target, gate, simulation);

Call to apply single controlled gate (eg. c-not)

    apply_controlled_gate(target, control, x, simulation);

Call to apply doubly controlled gate (eg. Toffoli)

    apply_double_controlled_gate(target, control1, int, control2, simulation);

Call to measure qubits

    measure(simulation);

Call to print results

    void print_results(simulation);

Call to test state_vector. Useful for debugging quantum algorithms.
    float state_vector[simulation->num_amp*2];

    void test_state_vector(state_vector, simulation);

## Example simulation

The following code performs a simulation (Figure 3: Example Circuit)

    int QUBITS = 3;

    Simulation *simulation = set_up_simulation();
    initialise_qubits(QUBITS, simulation);

    for(int i<0; i<QUBITS; i++)
        apply_gate(i, hadamard, simulation);

    apply_controlled_gate(1, 0, x, simulation);
    apply_controlled_gate(2, 1, x, simulation);

    measure(simulation);
    print_results(simulation);
    deallocate_resources(simulation);