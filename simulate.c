#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "program.c"
#define APPLY_GATE_FUNC "apply_gate"
#define APPLY_CGATE_FUNC "apply_controlled_gate"
#define MEASURE_FUNC "measure"
#define INITIALISE_FUNC "initialise_state"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <OpenCL/cl.h>
#include <math.h>

// Host and device data structures
// Put into struct and pass around to remove global variables
static cl_context context;
static cl_device_id device;
static cl_command_queue queue;
static cl_program program;
static cl_ulong global_mem_size;
static cl_kernel apply_gate_kernel;
static cl_kernel apply_controlled_gate_kernel;
static cl_kernel measure_kernel;
static cl_kernel initialise_state_kernel;
static float *state_vector;
static float *probabilities;
static size_t num_amp;
static cl_mem probability_buffer;
static cl_mem state_vector_buffer;

int int_to_int(int k) {
    return (k == 0 || k == 1 ? k : ((k % 2) + 10 * int_to_int(k / 2)));
}

void print_results()
{
    printf("Probabilities:\n");
    for(int i=0; i<num_amp; i++)
        if(probabilities[i] != 0)
            printf("%04d: %f %%\n", int_to_int(i), probabilities[i]*100);
}

void test_state_vector(float *state)
{
    cl_int error;

    error = clEnqueueReadBuffer(queue, state_vector_buffer, CL_TRUE, 0,
        sizeof(float)*num_amp*2, state, 0, NULL, NULL);
    if(error < 0) {
        printf("error: %d\n", error);
        perror("Couldn't enqueue the read state command");
        exit(1);   
    }
    
    return;
}

void deallocate_resources()
{
    free(state_vector);
    free(probabilities);
    clReleaseKernel(apply_gate_kernel);
    clReleaseKernel(apply_controlled_gate_kernel);
    clReleaseKernel(measure_kernel);
    clReleaseKernel(initialise_state_kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    
    return;
}

void measure()
{
    const size_t num_op = num_amp;
    cl_int error;

    error = clEnqueueNDRangeKernel(queue, measure_kernel, 1, NULL,
        &num_op, NULL, 0, NULL, NULL);
    if(error < 0) {
        perror("Couldn't enqueue the measure execution command");
        exit(1); 
    }

    error = clEnqueueReadBuffer(queue, probability_buffer, CL_TRUE, 0, 
        sizeof(float)*num_amp, probabilities, 0, NULL, NULL);
    if(error < 0) {
        perror("Couldn't enqueue the read buffer command");
        exit(1);   
    }
    
    return;
}

void apply_controlled_gate(int target, int controlled, float *gate)
{
    cl_int error;
    float *a, *b, *c, *d;
    const size_t num_op = num_amp;

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
    error = clSetKernelArg(apply_controlled_gate_kernel, 1, sizeof(int), &controlled);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's controlled argument");
        exit(1);
    }
    
    error = clSetKernelArg(apply_controlled_gate_kernel, 2, sizeof(int), &target);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's target argument");
        exit(1);
    }

    error = clSetKernelArg(apply_controlled_gate_kernel, 3, sizeof(float)*2, a);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's a argument");
        exit(1);
    }

    error = clSetKernelArg(apply_controlled_gate_kernel, 4, sizeof(float)*2, b);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's b argument");
        exit(1);
    }

    error = clSetKernelArg(apply_controlled_gate_kernel, 5, sizeof(float)*2, c);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's c argument");
        exit(1);
    }

    error = clSetKernelArg(apply_controlled_gate_kernel, 6, sizeof(float)*2, d);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's d argument");
        exit(1);
    }

    clEnqueueNDRangeKernel(queue, apply_controlled_gate_kernel, 1, NULL,
        &num_op, NULL, 0, NULL, NULL);

    free(a);
    free(b);
    free(c);
    free(d);
}

void apply_gate(int target, float *gate)
{
    cl_int error;
    float *a, *b, *c, *d;
    const size_t num_op = num_amp;

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
    error = clSetKernelArg(apply_gate_kernel, 1, sizeof(int), &target);
    if(error < 0) {
        perror("Couldn't set apply_gate's target argument");
        exit(1);
    }

    error = clSetKernelArg(apply_gate_kernel, 2, sizeof(float)*2, a);
    if(error < 0) {
        perror("Couldn't set apply_gate's a argument");
        exit(1);
    }

    error = clSetKernelArg(apply_gate_kernel, 3, sizeof(float)*2, b);
    if(error < 0) {
        perror("Couldn't set apply_gate's b argument");
        exit(1);
    }

    error = clSetKernelArg(apply_gate_kernel, 4, sizeof(float)*2, c);
    if(error < 0) {
        perror("Couldn't set apply_gate's c argument");
        exit(1);
    }

    error = clSetKernelArg(apply_gate_kernel, 5, sizeof(float)*2, d);
    if(error < 0) {
        perror("Couldn't set apply_gate's d argument");
        exit(1);
    }

    clEnqueueNDRangeKernel(queue, apply_gate_kernel, 1, NULL,
        &num_op, NULL, 0, NULL, NULL);

    free(a);
    free(b);
    free(c);
    free(d);

    return;
}

void initialise_qubits(int num_qubits)
{
    cl_int error;

    // Check the number of qubits is appropriate
    // Max seems to be 13 but should be 27 accoding to device
    // up to 12 qubits can be handled with a single work item per amplitude
    if(num_qubits > log2(global_mem_size/8) | num_qubits < 0) {
        printf("Invalid number of qubits: Maximum %d\n",
            (int) log2(global_mem_size/8));
        exit(1);
    }

    // Initialise state_vector data to |0....0>
    num_amp = pow(2, num_qubits);
    state_vector = (float *) calloc(num_amp*2, sizeof(float));
    state_vector[0] = 1;

    // Initialise probabilities buffer
    probabilities = (float *) malloc(sizeof(float)*num_amp);

    // Create CL buffer to hold the state vector
    state_vector_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
        CL_MEM_COPY_HOST_PTR, sizeof(float)*num_amp*2, state_vector, &error);
    if(error < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }

    // Create CL buffer to hold the measurement outcome
    probability_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
        sizeof(float)*num_amp, NULL, &error);
    if(error < 0) {
        perror("Couldn't create a buffer object");
        exit(1);
    }

    // Set state vector buffer kernel arguments
    error = clSetKernelArg(apply_gate_kernel, 0,
        sizeof(cl_mem), &state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set apply_gate's state_vector argument");
        exit(1);
    }

    error = clSetKernelArg(apply_controlled_gate_kernel, 0,
        sizeof(cl_mem), &state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set apply_controlled_gate's state_vector argument");
        exit(1);
    }

    // Set kernel arguments for measure
    error = clSetKernelArg(measure_kernel, 0, 
        sizeof(cl_mem), &state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set measure's state_vector argument");
        exit(1);
    }

    error = clSetKernelArg(measure_kernel, 1,
        sizeof(cl_mem), &probability_buffer);
    if(error < 0) {
        perror("Couldn't set measure's probability argument");
        exit(1);
    }

    // Set kernel argument for initialise_state
    error = clSetKernelArg(initialise_state_kernel, 0,
        sizeof(cl_mem), &state_vector_buffer);
    if(error < 0) {
        perror("Couldn't set initialise_state's state_vector argument");
        exit(1);
    }
}

void set_up_simulator()
{
    // Data structures
    cl_int error, max_work_item_dims;
    cl_uint max_compute_units;
    cl_platform_id platform;
    char *program_buffer, *program_log;
    size_t program_size, log_size, max_work_group_size;
    size_t *max_work_item_size;
    FILE *fp;

    // Identify a platform
    error = clGetPlatformIDs(1, &platform, NULL);
    if(error < 0) {
        perror("No platforms found");
        exit(1);
    }

    // Access a GPU
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if(error < 0) {
        perror("No GPU found");
        exit(1);
    }

    // Check the GPU's maximum number of work dimensions
    error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        sizeof(cl_int), &max_work_item_dims, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work item dimensions");
        exit(1);
    }
    //printf("Maximum work item dimensions: %u\n", max_work_item_dims);

    // Check the GPU's maximum number of work group size
    error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
        sizeof(size_t), &max_work_group_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work group size");
        exit(1);
    }
    //printf("Maximum work group size: %lu\n", max_work_group_size);

    // Check the GPU's maximum number of work item sizes
    max_work_item_size = (size_t *) malloc(max_work_item_dims*sizeof(size_t));
    error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
        sizeof(size_t)*max_work_item_dims, max_work_item_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's max work item size");
        exit(1);
    }
    //for(int i=0; i<max_work_item_dims; i++)
        //printf("Maximum work item %d size: %lu\n", i, max_work_item_size[i]);
    free(max_work_item_size);

    // Check the GPU's number of compute units
    error = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS,
        sizeof(cl_uint), &max_compute_units, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's number of compute units");
        exit(1);
    }
    //printf("Maximum compute units: %d\n", max_compute_units);

    // Check the GPU's global memory size
    error = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE,
        sizeof(cl_ulong), &global_mem_size, NULL);
    if(error < 0) {
        perror("Couldn't access GPU's global memory size");
        exit(1);
    }
    //printf("Global memory size: %llu bytes\n", global_mem_size);

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
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
    program = clCreateProgramWithSource(context, 1,
        (const char **) &program_buffer, &program_size, &error);
    if(error < 0) {
        perror("Couldn't create program");
        exit(1);
    }
    free(program_buffer);

    // Build program
    error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if(error < 0) {
        // Determine size of log and allocate buffer space
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            0, NULL, &log_size);
        program_log = (char *) malloc(log_size + 1);
        program_log[log_size] = '\0';
        
        // Copy log into buffer and print
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    // Create kernel for the apply_gate function
    apply_gate_kernel = clCreateKernel(program, APPLY_GATE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the apply gate kernel");
        exit(1);
    }

    // Create kernel for the apply_controlled_gate function
    apply_controlled_gate_kernel = clCreateKernel(program, 
        APPLY_CGATE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the apply controlled gate kernel");
        exit(1);
    }

    // Create kernel for the measure function
    measure_kernel = clCreateKernel(program, MEASURE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the measure kernel");
        exit(1);
    }

    // Create kernel for the initialise_state function
    initialise_state_kernel = clCreateKernel(program, INITIALISE_FUNC, &error);
    if(error < 0) {
        perror("Couldn't create the initialise state kernel");
        exit(1);
    }

    // Create command queue for the GPU
    queue = clCreateCommandQueue(context, device, 0, &error);
    if(error < 0) {
        perror("Couldn't create the command queue");
        exit(1);
    }

    return;
}

int main()
{
    float not[8] = {0,0,1,0,1,0,0,0};
    float hadamard[8] = {1/sqrt(2),0,1/sqrt(2),0,1/sqrt(2),0,-1/sqrt(2),0};

    set_up_simulator();
    initialise_qubits(4);

    apply_gate(0, hadamard);
    apply_gate(1, not);
    apply_gate(3, not);
    apply_gate(1, hadamard);
    apply_controlled_gate(2, 1, not);
    apply_controlled_gate(1, 0, not);
    apply_gate(1, not);

    measure();
    print_results();
    deallocate_resources();

    return 0;
}
