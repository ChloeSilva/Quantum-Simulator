typedef float2 cfloat;

inline cfloat cmult(cfloat a, cfloat b)
{
    return (cfloat)( a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

inline float cabsolute(cfloat a)
{
    return sqrt((a.x * a.x) + (a.y * a.y));
}

inline int get_index(int n, int target)
{
    int mask = (1 << target) - 1;
    return (n & mask) | ((n & ~mask) << 1);
}

__kernel void apply_gate(__global cfloat *state_vector, int target,
    cfloat A, cfloat B, cfloat C,cfloat D)
{
    int const global_id = get_global_id(0);

    int const zero_state = get_index(global_id, target);
    int const one_state = zero_state | (1 << target);

    cfloat const zero_amp = state_vector[zero_state];
    cfloat const one_amp = state_vector[one_state];

    state_vector[zero_state] = cmult(A, zero_amp) + cmult(B, one_amp);
    state_vector[one_state] = cmult(C, zero_amp) + cmult(D, one_amp);
}

__kernel void apply_controlled_gate(__global cfloat *state_vector,
    int control, int target, cfloat A, cfloat B, cfloat C, cfloat D)
{
    int const global_id = get_global_id(0);

    int const zero_state = get_index(global_id, target);
    int const one_state = zero_state | (1 << target);

    int const control_val_zero = (((1 << control) & zero_state) > 0) ? 1 : 0;
    int const control_val_one = (((1 << control) & one_state) > 0) ? 1 : 0;

    cfloat const zero_amp = state_vector[zero_state];
    cfloat const one_amp = state_vector[one_state];

    if (control_val_zero == 1)
        state_vector[zero_state] = cmult(A, zero_amp) + cmult(B, one_amp);
    
    if (control_val_one == 1)
        state_vector[one_state] = cmult(C, zero_amp) + cmult(D, one_amp);
}

__kernel void apply_controlled_controlled_gate(__global cfloat *amplitudes,
    int control_1, int control_2, int target, cfloat A, cfloat B, cfloat C,
    cfloat D)
{
    int const global_id = get_global_id(0);

    int const zero_state = get_index(global_id, target);
    int const one_state = zero_state | (1 << target);

    int const control_val_zero = (((1 << control_1) & zero_state) > 0) ? 1 : 0;
    int const control_val_one = (((1 << control_1) & one_state) > 0) ? 1 : 0;

    int const control_val_two_zero = (((1 << control_2) & zero_state) > 0) ? 1 : 0;
    int const control_val_two_one = (((1 << control_2) & one_state) > 0) ? 1 : 0;

    cfloat const zero_amp = amplitudes[zero_state];
    cfloat const one_amp = amplitudes[one_state];
    
    if (control_val_zero == 1 && control_val_two_zero == 1)
        amplitudes[zero_state] = cmult(A, zero_amp) + cmult(B, one_amp);
    
    if (control_val_one == 1 && control_val_two_one == 1)
        amplitudes[one_state] = cmult(D, one_amp) + cmult(C, zero_amp);
}

__kernel void measure(__global cfloat *const state_vector,
                      __global float *probabilities)
{
    int const index = get_global_id(0);
    cfloat amp = state_vector[index];
    probabilities[index] = cabsolute(cmult(amp, amp));
}

__kernel void initialise_state(__global cfloat *state_vector)
{
    int const index = get_global_id(0);
    state_vector[index][0] = 0;
    state_vector[index][1] = 0;
}