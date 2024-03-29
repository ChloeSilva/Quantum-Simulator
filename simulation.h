#ifndef _SIMULATION_H
#define _SIMULATION_H

#include <OpenCL/cl.h>

extern const float sqrt_2;
extern float x[8];
extern float z[8];
extern float hadamard[8];

typedef struct Simulation
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_ulong global_mem_size;
    cl_kernel apply_gate_kernel;
    cl_kernel apply_controlled_gate_kernel;
    cl_kernel apply_double_controlled_gate_kernel;
    cl_kernel measure_kernel;
    cl_kernel initialise_state_kernel;
    float *state_vector;
    float *probabilities;
    size_t num_amp;
    float epsilon;
    cl_mem probability_buffer;
    cl_mem state_vector_buffer;
} Simulation;

void print_results(Simulation *);
void test_state_vector(float *, Simulation *);
void deallocate_resources(Simulation *);
void measure(Simulation *);
void apply_gate(int, float[8], Simulation *);
void apply_controlled_gate(int, int, float[8], Simulation *);
void apply_double_controlled_gate(int, int, int, float[8], Simulation *);
void initialise_qubits(int, Simulation *);
Simulation *set_up_simulation(void);

#endif
