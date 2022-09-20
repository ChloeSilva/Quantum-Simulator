#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "program.cl"
#define APPLY_GATE_FUNC "apply_gate"
#define APPLY_CGATE_FUNC "apply_controlled_gate"
#define APPLY_CCGATE_FUNC "apply_double_controlled_gate"
#define MEASURE_FUNC "measure"
#define INITIALISE_FUNC "initialise_state"

#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <OpenCL/cl.h>
#include <math.h>

const float sqrt_2 = 1.414214;
float x[8] = {0,0,1,0,1,0,0,0};
float z[8] = {1,0,0,0,0,0,1,0};
float hadamard[8] = {1/sqrt_2,0,1/sqrt_2,0,1/sqrt_2,0,-1/sqrt_2,0};

/**
 * Sets a given array of size n to the binary representation of k
 * @param k An integer to be converted into binary
 * @param n The size of the array containing the binary representation
 * @param array The array in which to store the binary representation
 */
void int_to_bin(int k, int n, char *array) {
    for(int i=n-1; i>=0; i--)
        if(k >= (int) pow((double) 2, (double) i)) {
            array[i] = '1';
            k -= (int) pow((double) 2, (double) i);
        } else
            array[i] = '0';

    array[n] = '\0';
}
/**
 * Prints the results of a given simulation.
 * To chose the threshold for which to print a value, set simulation's epsilon value.
 * @param simulation The current simulation object to print from
 */
void print_results(Simulation *simulation)
{
    char array[(int) log2(simulation->num_amp)+1];

    for(int i=0; i<simulation->num_amp; i++)
        if(simulation->probabilities[i] >= simulation->epsilon) {
            int_to_bin(i, log2(simulation->num_amp), array);
            //printf("%s: %f %%\n", array, simulation->probabilities[i]*100);
            printf("%s\n", array);
        }
}

/**
 * Reads the state vector from the GPU'memory and stores it in the state array.
 * Useful for debugging quantum algorithms.
 * @param state An array to hold the state buffer, must be at least of size
 * simulation->num_amplitudes * 2
 * @param simulation The simulation to test
 */
void test_state_vector(float *state, Simulation *simulation)
{
    cl_int error;

    error = clEnqueueReadBuffer(simulation->queue, simulation->state_vector_buffer,
        CL_TRUE, 0, sizeof(float)*simulation->num_amp*2, state, 0, NULL, NULL);
    if(error < 0) {
        printf("error: %d\n", error);
        perror("Couldn't enqueue the read state command");
        exit(1);   
    }
    
    return;
}

/**
 * Deallocates all resources associated with a simulation.
 * Run before exiting program to avoid memory leaks.
 * @param simulation A simulation object which is no longer in use
 */
void deallocate_resources(Simulation *simulation)
{
    if(simulation->apply_gate_kernel)
        clReleaseKernel(simulation->apply_gate_kernel);
    
    if(simulation->apply_controlled_gate_kernel)
        clReleaseKernel(simulation->apply_controlled_gate_kernel);
    
    if(simulation->apply_double_controlled_gate_kernel)
        clReleaseKernel(simulation->apply_double_controlled_gate_kernel);

    if(simulation->measure_kernel)
        clReleaseKernel(simulation->measure_kernel);

    if(simulation->initialise_state_kernel)
        clReleaseKernel(simulation->initialise_state_kernel);

    if(simulation->queue)
        clReleaseCommandQueue(simulation->queue);

    if(clReleaseProgram(simulation->program))
        clReleaseProgram(simulation->program);

    if(clReleaseContext(simulation->context))
        clReleaseContext(simulation->context);

    if(simulation->state_vector)
        free(simulation->state_vector);

    if(simulation->probabilities)
        free(simulation->probabilities);
    
    free(simulation);
    
    return;
}

/**
 * Meaures all registers of a simulation and stores the probabilities of a
 * qubit collapsing to each state in the simulation's probabilities array.
 * @param simulation The simulation to be measured
 */
void measure(Simulation *simulation)
{
    const size_t num_op = simulation->num_amp;
    cl_int error;

    error = clEnqueueNDRangeKernel(simulation->queue, simulation->measure_kernel, 
        1, NULL, &num_op, NULL, 0, NULL, NULL);
    
    if(error < 0) {
        perror("Couldn't enqueue the measure execution command");
        exit(1); 
    }

    error = clEnqueueReadBuffer(simulation->queue, simulation->probability_buffer,
        CL_TRUE, 0, sizeof(float)*simulation->num_amp, simulation->probabilities,
        0, NULL, NULL);
    
    if(error < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);   
    }
    
    return;
}

/**
 * Sets kernel arguments and queues the apply_double_controlled_gate() kernel.
 * May accept any gate type and applies the doubly controlled version of the
 * gate on the specified target and control qubits. May be used with the
 * NOT gate to apply a toffoli gate.
 * @param target The target qubit for the gate
 * @param control_1 The first control qubit for the gate
 * @param control_2 The seccond control qubit for the gate
 * @param gate An array containing the matrix of the gate to be applied on the
 * target qubit
 * @param simulation The simulation on which to apply the gate
 */
void apply_double_controlled_gate(int target, int control_1, int control_2,
    float gate[8], Simulation *simulation)
{
    cl_int error;
    float *a, *b, *c, *d;
    const size_t num_op = simulation->num_amp;

    a = (float *) malloc(sizeof(float)*2);
    a[0] = gate[0];
    a[1] = gate[1];

    b = (float *) malloc(sizeof(float)*2);
    b[0] = gate[2];
    b[1] = gate[3];
    
    c = (float *) malloc(sizeof(float)*2);
    c[0] = gate[4];
    c[1] = gate[5];

    d = (float *) malloc(sizeof(float)*2);
    d[0] = gate[6];
    d[1] = gate[7];
    
    // Set kernel arguments for apply_controlled_gate
    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        1, sizeof(int), &control_1);

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        2, sizeof(int), &control_2);
    
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's controlled argument");
        exit(1);
    }
    
    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        3, sizeof(int), &target);
    
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's target argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        4, sizeof(float)*2, a);
    
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's a argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        5, sizeof(float)*2, b);
    
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's b argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        6, sizeof(float)*2, c);
    
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's c argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 
        7, sizeof(float)*2, d);
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's d argument");
        exit(1);
    }

    // queue the kernel
    clEnqueueNDRangeKernel(simulation->queue, 
        simulation->apply_double_controlled_gate_kernel, 
        1, NULL, &num_op, NULL, 0, NULL, NULL);

    free(a);
    free(b);
    free(c);
    free(d);
}

/**
 * Sets kernel arguments and queues the apply_controlled_gate() kernel.
 * May accept any gate type and applies the controlled version of the
 * gate on the specified target and control qubits. May be used with the
 * NOT gate to apply a c-not gate for example.
 * @param target The target qubit for the gate
 * @param controlled The control qubit for the gate
 * @param gate An array containing the matrix of the gate to be applied on the
 * target qubit
 * @param simulation The simulation on which to apply the gate
 */
void apply_controlled_gate(int target, int controlled, float gate[8],
    Simulation *simulation)
{
    cl_int error;
    float *a, *b, *c, *d;
    const size_t num_op = simulation->num_amp;

    a = (float *) malloc(sizeof(float)*2);
    a[0] = gate[0];
    a[1] = gate[1];

    b = (float *) malloc(sizeof(float)*2);
    b[0] = gate[2];
    b[1] = gate[3];
    
    c = (float *) malloc(sizeof(float)*2);
    c[0] = gate[4];
    c[1] = gate[5];

    d = (float *) malloc(sizeof(float)*2);
    d[0] = gate[6];
    d[1] = gate[7];
    
    // Set kernel arguments for apply_controlled_gate
    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        1, sizeof(int), &controlled);
    
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's controlled argument");
        exit(1);
    }
    
    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        2, sizeof(int), &target);
    
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's target argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        3, sizeof(float)*2, a);
    
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's a argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        4, sizeof(float)*2, b);
    
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's b argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        5, sizeof(float)*2, c);
    
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's c argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 
        6, sizeof(float)*2, d);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's d argument");
        exit(1);
    }

    // queue kerenel
    clEnqueueNDRangeKernel(simulation->queue, 
        simulation->apply_controlled_gate_kernel, 
        1, NULL, &num_op, NULL, 0, NULL, NULL);

    free(a);
    free(b);
    free(c);
    free(d);
}

/**
 * Sets kernel arguments and queues the apply_gate() kernel.
 * May accept any gate type and applies the gate on the specified target qubit.
 * @param target The target qubit for the gate
 * @param gate An array containing the matrix of the gate to be applied on the
 * target qubit
 * @param simulation The simulation on which to apply the gate
 */
void apply_gate(int target, float gate[8], Simulation *simulation)
{
    cl_int error;
    float *a, *b, *c, *d;
    const size_t num_op = simulation->num_amp;

    a = (float *) malloc(sizeof(float)*2);
    a[0] = gate[0];
    a[1] = gate[1];

    b = (float *) malloc(sizeof(float)*2);
    b[0] = gate[2];
    b[1] = gate[3];
    
    c = (float *) malloc(sizeof(float)*2);
    c[0] = gate[4];
    c[1] = gate[5];

    d = (float *) malloc(sizeof(float)*2);
    d[0] = gate[6];
    d[1] = gate[7];

    // Set kernel arguments for apply_gate
    error = clSetKernelArg(simulation->apply_gate_kernel, 1, sizeof(int), &target);
    if(error < 0) {
        perror("Couldn't set apply_gate's target argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_gate_kernel, 2, sizeof(float)*2, a);
    if(error < 0) {
        perror("Couldn't set apply_gate's a argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_gate_kernel, 3, sizeof(float)*2, b);
    if(error < 0) {
        perror("Couldn't set apply_gate's b argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_gate_kernel, 4, sizeof(float)*2, c);
    if(error < 0) {
        perror("Couldn't set apply_gate's c argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_gate_kernel, 5, sizeof(float)*2, d);
    if(error < 0) {
        perror("Couldn't set apply_gate's d argument");
        exit(1);
    }

    clEnqueueNDRangeKernel(simulation->queue, simulation->apply_gate_kernel, 1, NULL,
        &num_op, NULL, 0, NULL, NULL);

    free(a);
    free(b);
    free(c);
    free(d);

    return;
}

/**
 * Call to initialise qubits.
 * Creates a state vector of appropriate size to store the simulation's
 * amplitudes and sets it as a kernel argument to all required kernels
 * @param num_qubits The number of qubits to initialise
 * @param simulation The simulation object to initialise
 */
void initialise_qubits(int num_qubits, Simulation *simulation)
{
    cl_int error;

    // Check the GPU has enough global memory for the number of qubits
    if(num_qubits > log2(simulation->global_mem_size/8) | num_qubits < 0) {
        printf("Invalid number of qubits: Maximum %d\n",
            (int) log2(simulation->global_mem_size/8));
        exit(1);
    }

    // Initialise state_vector data to |0....0>
    simulation->num_amp = pow(2, num_qubits);
    simulation->state_vector = (float *) calloc(simulation->num_amp*2, sizeof(float));
    simulation->state_vector[0] = 1;

    // Initialise probabilities buffer
    simulation->probabilities = (float *) malloc(sizeof(float)*simulation->num_amp);

    // Create CL buffer to hold the state vector
    simulation->state_vector_buffer = clCreateBuffer(simulation->context, CL_MEM_READ_WRITE |
        CL_MEM_COPY_HOST_PTR, sizeof(float)*simulation->num_amp*2, simulation->state_vector, &error);
    if(error < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }

    // Create CL buffer to hold the measurement outcome
    simulation->probability_buffer = clCreateBuffer(simulation->context, CL_MEM_READ_ONLY,
        sizeof(float)*simulation->num_amp, NULL, &error);
    if(error < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }

    // Set state vector buffer kernel arguments
    error = clSetKernelArg(simulation->apply_gate_kernel, 0,
        sizeof(cl_mem), &simulation->state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set apply_gate's state_vector argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_controlled_gate_kernel, 0,
        sizeof(cl_mem), &simulation->state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's state_vector argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->apply_double_controlled_gate_kernel, 0,
        sizeof(cl_mem), &simulation->state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set apply_double_controlled_gate's state_vector argument");
        exit(1);
    }

    // Set kernel arguments for measure
    error = clSetKernelArg(simulation->measure_kernel, 0, 
        sizeof(cl_mem), &simulation->state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set measure's state_vector argument");
        exit(1);
    }

    error = clSetKernelArg(simulation->measure_kernel, 1,
        sizeof(cl_mem), &simulation->probability_buffer);
    if(error < 0) {
        perror("Couldn't set measure's probability argument");
        exit(1);
    }

    // Set kernel argument for initialise_state
    error = clSetKernelArg(simulation->initialise_state_kernel, 0,
        sizeof(cl_mem), &simulation->state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set initialise_state's state_vector argument");
        exit(1);
    }
}
/**
 * @brief Set the up simulation object.
 * Complies the OpenCL program, creates all kernel function, defines
 * context, creates command queue, sets known kernel arguments and
 * verifies GPU capabilities.
 * @return simulation object containing all OpenCL objects
 */
Simulation *set_up_simulation()
{
    // Data structures
    Simulation *simulation;
    cl_int error, max_work_item_dims;
    cl_uint max_compute_units;
    cl_platform_id platform;
    char *program_buffer, *program_log;
    size_t program_size, log_size, max_work_group_size;
    size_t *max_work_item_size;
    FILE *fp;

    // Initialise Simulation struct
    simulation = (Simulation *) malloc(sizeof(Simulation));
    if(!simulation) {
        fprintf(stderr, "error: unable to initialise Simulation.\n");
        exit(EXIT_FAILURE);
    }

    // Identify a platform
    error = clGetPlatformIDs(1, &platform, NULL);
    if(error < 0) {
        perror("No platforms found");
        exit(1);
    }

    // Access a GPU
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1,
        &simulation->device, NULL);
    if(error < 0) {
        perror("No GPU found");
        exit(1);
    }

    // Check the GPU's maximum number of work dimensions
    error = clGetDeviceInfo(simulation->device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        sizeof(cl_int), &max_work_item_dims, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work item dimensions");
        exit(1);
    }
    // printf("Maximum work item dimensions: %u\n", max_work_item_dims);

    // Check the GPU's maximum number of work group size
    error = clGetDeviceInfo(simulation->device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
        sizeof(size_t), &max_work_group_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work group size");
        exit(1);
    }
    // printf("Maximum work group size: %lu\n", max_work_group_size);

    // Check the GPU's maximum number of work item sizes
    max_work_item_size = (size_t *) malloc(max_work_item_dims*sizeof(size_t));
    error = clGetDeviceInfo(simulation->device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
        sizeof(size_t)*max_work_item_dims, max_work_item_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work item size");
        exit(1);
    }
    // for(int i=0; i<max_work_item_dims; i++)
    //     printf("Maximum work item %d size: %lu\n", i, max_work_item_size[i]);
    free(max_work_item_size);

    // Check the GPU's number of compute units
    error = clGetDeviceInfo(simulation->device, CL_DEVICE_MAX_COMPUTE_UNITS,
        sizeof(cl_uint), &max_compute_units, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's number of compute units");
        exit(1);
    }
    //printf("Maximum compute units: %d\n", max_compute_units);

    // Check the GPU's global memory size
    error = clGetDeviceInfo(simulation->device, CL_DEVICE_GLOBAL_MEM_SIZE,
        sizeof(cl_ulong), &simulation->global_mem_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's global memory size");
        exit(1);
    }
    //printf("Global memory size: %llu bytes\n", simulation->global_mem_size);

    // Create context
    simulation->context = clCreateContext(NULL, 1, &simulation->device, NULL, NULL, &error);
    if(error < 0) {
        perror("Couldn't create context");
        exit(1);
    }

    // Open program file
    fp = fopen(PROGRAM_FILE, "r");
    if(fp == NULL) {
        perror("Couldn't find program file");
        exit(1);
    }
    
    // Determine size of program and allocate buffer space
    fseek(fp, 0, SEEK_END);
    program_size = ftell(fp);
    program_buffer = (char *) malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    
    // Read program file into buffer
    rewind(fp);
    fread(program_buffer, sizeof(char), program_size, fp);
    fclose(fp);

    // Create program
    simulation->program = clCreateProgramWithSource(simulation->context, 1,
        (const char **) &program_buffer, &program_size, &error);
    if(error < 0) {
        perror("Couldn't create program");
        exit(1);
    }
    free(program_buffer);

    // Build program
    error = clBuildProgram(simulation->program, 0, NULL, NULL, NULL, NULL);
    if(error < 0) {
        // Determine size of log and allocate buffer space
        clGetProgramBuildInfo(simulation->program, simulation->device,
            CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char *) malloc(log_size + 1);
        program_log[log_size] = '\0';
        
        // Copy log into buffer and print
        clGetProgramBuildInfo(simulation->program, simulation->device,
            CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    // Create kernel for the apply_gate function
    simulation->apply_gate_kernel = clCreateKernel(simulation->program, APPLY_GATE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the apply gate kernel");
        exit(1);
    }

    // Create kernel for the apply_controlled_gate function
    simulation->apply_controlled_gate_kernel = clCreateKernel(simulation->program, 
        APPLY_CGATE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the apply controlled gate kernel");
        exit(1);
    }

    // Create kernel for the apply_double_controlled_gate function
    simulation->apply_double_controlled_gate_kernel = 
        clCreateKernel(simulation->program, APPLY_CCGATE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the apply controlled controlled gate kernel");
        exit(1);
    }

    // Create kernel for the measure function
    simulation->measure_kernel = clCreateKernel(simulation->program, MEASURE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the measure kernel");
        exit(1);
    }

    // Create kernel for the initialise_state function
    simulation->initialise_state_kernel = clCreateKernel(simulation->program, INITIALISE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the initialise state kernel");
        exit(1);
    }

    // Create command queue for the GPU
    simulation->queue = clCreateCommandQueue(simulation->context, simulation->device, 0, &error);
    if(error < 0) {
        perror("Couldn't create the command queue");
        exit(1);
    }

    // Initialise epsilon
    simulation->epsilon = 0;

    return simulation;
}
