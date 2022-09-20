#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define QUBITS 5

int gcd(int a, int b)
{
    return 1;
}

int continued_fractions(int n)
{
    return 4;
}

int num_factors(int n)
{
    int answer = 0;
    for(int i = 2; i <= n/2; i++)
        if(n % i == 0)
            answer++;

    return answer;
}

void test_num_factors()
{
    // given
    int a = 15;
    int b = 12;

    // when
    int a_num_factors = num_factors(a);
    int b_num_factors = num_factors(b);

    // then
    printf("%d num_factors: %d\n", a, a_num_factors);
    printf("%d num_factors: %d\n", b, b_num_factors);
}

int request_user_input()
{
    int input, error;
    char c = '\0';

    printf("Please enter a number between 0-%d to factorise: ", (int) pow(2, QUBITS-1)-1);
    error = scanf("%d",&input);
    
    if (error == EOF || error == 0 || input  < 0 || input > pow(2, QUBITS-1)-1) 
    {
        printf("\nIncorrect input: ");
        while(c != '\n')
            c = getchar();

        return request_user_input();
    } else if(num_factors(input) > 2) {
        printf("%d has more than 2 factors, please select a different number to apply shor's algorithm.\n", input);
        while(c != '\n')
            c = getchar();

        return request_user_input();
    } else {
        return input;
    }
}

void int_to_binary(int n, int *binary_list)
{
    for(int i=0; i<QUBITS; i++) {
        if(n % 2 == 1) {
            binary_list[i] = 1;
            n -= 1;
            n /=2;
        } else {
            binary_list[i] = 0;
            n /=2;
        }
    }
}

void reverse(int *list)
{
    int temp;

    for(int i=0; i<QUBITS/2; i++) {
        temp = list[i];
        list[i] = list[QUBITS-1-i];
        list[QUBITS-1-i] = temp;
    }

}

int mod_inverse(int a, int n)
{
	int n_0 = n;
	int x_0 = 0; 
    int x_1 = 1;
    int t, q;
    
	if (n == 1) 
        return 1;
	
    while (a > 1) {
		q = a / n;
		t = n, n = a % n, a = t;
		t = x_0, x_0 = x_1 - q * x_0, x_1 = t;
	}
	if (x_1 < 0)
        x_1 += n_0;
	
    return x_1;
}

void phase_gate(float k, float gate[8])
{   
    float values[8] = { 1, 0, 0, 0, 0, 0, cosf(k), sinf(k)};
    
    for(int i=0; i<8; i++)
        gate[i] = values[i];
}

void apply_swap(int a, int b, Simulation *simulation)
{
    apply_controlled_gate(a, b, x, simulation);
    apply_controlled_gate(b, a, x, simulation);
    apply_controlled_gate(a, b, x, simulation);
}

void apply_multi_swap(Simulation *simulation)
{
    for(int i=0; i<QUBITS-1; i++)
        apply_swap(i, QUBITS+i, simulation);
}

void apply_qft(int size, Simulation *simulation)
{
    float gate[8];

    for(int i=0; i<size; i++) {
        for(int j=size; j>i; j--) {
            if(j == size) {
                apply_gate(i, hadamard, simulation);
            } else {
                phase_gate(M_PI/pow(2, j-i), gate);
                apply_controlled_gate(i, j, gate, simulation);
            }
        }
    }

    for(int i=0; i<size/2; i++) {
        apply_swap(i, size-i-1, simulation);
    }
}

void apply_inv_qft(int size, Simulation *simulation)
{
    float gate[8];

    for(int i=0; i<size/2; i++) {
        apply_swap(i, size-i-1, simulation);
    }

    for(int i=0; i<size; i++) {
        for(int j=0; j<=i; j++) {
            if(j == i) {
                apply_gate(size-(i+1), hadamard, simulation);
            } else {
                phase_gate(-M_PI/pow(2, j+1), gate);
                apply_controlled_gate(size-(i+1), size-i+j, gate, simulation);
            }
        }
    }
}

void apply_add(int a, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=QUBITS; i>0; i--) {
        for(int j=0; j<i; j++) {
            if(binary_a[i-j-1]) {
                phase_gate(2*M_PI/pow(2, j+1), gate);
                apply_gate(i-1, gate, simulation);
            }
        }
    }
}

void apply_inv_add(int a, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=0; i<QUBITS; i++) {
        for(int j=i; j>=0; j--) {
            if(binary_a[i-j]) {
                phase_gate(-2*M_PI/pow(2, j+1), gate);
                apply_gate(i, gate, simulation);
            }
        }
    }
}

void apply_add_c(int a, int c, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=QUBITS; i>0; i--) {
        for(int j=0; j<i; j++) {
            if(binary_a[i-j-1]) {
                phase_gate(2*M_PI/pow(2, j+1), gate);
                apply_controlled_gate(i-1, c, gate, simulation);
            }
        }
    }
}

void apply_inv_add_c(int a, int c, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=QUBITS; i>0; i--) {
        for(int j=0; j<i; j++) {
            if(binary_a[i-j-1]) {
                phase_gate(-2*M_PI/pow(2, j+1), gate);
                apply_controlled_gate(i-1, c, gate, simulation);
            }
        }
    }
}

void apply_add_c_c(int a, int c1, int c2, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=QUBITS; i>0; i--) {
        for(int j=0; j<i; j++) {
            if(binary_a[i-j-1]) {
                phase_gate(2*M_PI/pow(2, j+1), gate);
                apply_double_controlled_gate(i-1, c1, c2, gate, simulation);
            }
        }
    }
}

void apply_inv_add_c_c(int a, int c1, int c2, Simulation *simulation)
{
    float gate[8];
    int binary_a[8];
    int_to_binary(a, binary_a);

    for(int i=0; i<QUBITS; i++) {
        for(int j=i; j>=0; j--) {
            if(binary_a[i-j]) {
                phase_gate(-2*M_PI/pow(2, j+1), gate);
                apply_double_controlled_gate(i, c1, c2, gate, simulation);
            }
        }
    }
}

void add_a_mod_n(int a, int n, int c1, int c2, int zero, Simulation *simulation)
{
    int sig = QUBITS-1;
    apply_add_c_c(a, c1, c2, simulation);
    apply_inv_add(n, simulation);
    apply_inv_qft(QUBITS, simulation);
    apply_controlled_gate(zero, sig, x, simulation);
    apply_qft(QUBITS, simulation);
    apply_add_c(n, zero, simulation);
    apply_inv_add_c_c(a, c1, c2, simulation);
    apply_inv_qft(QUBITS, simulation);
    apply_gate(sig, x, simulation);
    apply_controlled_gate(zero, sig, x, simulation);
    apply_gate(sig, x, simulation);
    apply_qft(QUBITS, simulation);
    apply_add_c_c(a, c1, c2, simulation);
}

void inv_add_a_mod_n(int a, int n, int c1, int c2, int zero, Simulation *simulation)
{
    int sig = QUBITS-1;
    apply_inv_add_c_c(a, c1, c2, simulation);
    apply_inv_qft(QUBITS, simulation);
    apply_gate(sig, x, simulation);
    apply_controlled_gate(zero, sig, x, simulation);
    apply_gate(sig, x, simulation);
    apply_qft(QUBITS, simulation);
    apply_add_c_c(a, c1, c2, simulation);
    apply_inv_add_c(n, zero, simulation);
    apply_inv_qft(QUBITS, simulation);
    apply_controlled_gate(zero, sig, x, simulation);
    apply_qft(QUBITS, simulation);
    apply_add(n, simulation);
    apply_inv_add_c_c(a, c1, c2, simulation);
}

void mult_a_mod_n_c(int a, int n, int c, int zero, Simulation *simulation)
{
    for(int i=0; i<QUBITS-1; i++)
        add_a_mod_n(pow(2, i)*a, n, c, QUBITS+i, zero, simulation);
}

void inv_mult_a_mod_n_c(int a, int n, int c, int zero, Simulation *simulation)
{
    for(int i=QUBITS-2; i>=0; i--)
        inv_add_a_mod_n(pow(2, i)*a, n, c, QUBITS+i, zero, simulation);
}

void apply_u_a(int a, int n, int c, int zero, Simulation *simulation)
{
    mult_a_mod_n_c(a, n, c, zero, simulation);
    apply_multi_swap(simulation);
    mod_inverse(a, n);
    inv_mult_a_mod_n_c(mod_inverse(a, n), n, c, zero, simulation);
}

int read_qubit(int n, Simulation *simulation)
{
    float state_vector[simulation->num_amp*2];
    test_state_vector(state_vector, simulation);

    for(int i=0; i<simulation->num_amp*2; i++)
        state_vector[i] *= state_vector[i];
    
    return 1;
}

int find_period(int a, int N)
{
    Simulation *simulation = set_up_simulation();
    initialise_qubits((QUBITS-1)*2+3, simulation);

    int phase;
    int ancilla = QUBITS*2-1;
    int answer = QUBITS*2;
    int answer_register[QUBITS-1];

    apply_gate(0, x, simulation);

    for(int i=0; i<QUBITS-1; i++) {
        apply_gate(answer, hadamard, simulation);
        apply_u_a((int) pow(a, pow(2, i)) % N, N, answer, ancilla, simulation);
        answer_register[i] = read_qubit(answer, simulation);
        apply_gate(answer, hadamard, simulation);
        if(answer_register[i] == 1)
            apply_gate(answer, x, simulation);
    }

    deallocate_resources(simulation);

    simulation = set_up_simulation();
    simulation->epsilon = 0.1;
    initialise_qubits(QUBITS-1, simulation);
    
    for(int i=0; i<QUBITS-1; i++) {
        apply_gate(i, hadamard, simulation);
        if(answer_register[i] == 1)
            apply_gate(i, x, simulation);
    }

    apply_qft(QUBITS-1, simulation);

    measure(simulation);
    
    for(int i=0; i<simulation->num_amp; i++)
        if((simulation->probabilities[i]*100) > simulation->epsilon)
            phase = i;

    deallocate_resources(simulation);
    
    return continued_fractions(phase);
}

int main()
{
    //seed rand
    int N = request_user_input();

    while(true) {
        int a = 7;
        int g;
        //int a = rand();
        printf("Selected integer a = %d\n", a);
        g = gcd(a, N);

        if(g != 1)
            printf("gcd(a, N) = 1: Found factors %d and %d", g, N/g);

        int r = find_period(a, N);
        
        printf("Found period r = %d\n", r);
        if(r % 2 == 1) {
            printf("r is even, selecting a new value of a\n");
            continue;
        } if((int) (pow(a, r/2) + 1) % N == 0) {
            printf("a^(r/2) + 1 is divisible by N, selecting a new value of a\n");
            continue;
        } else {
            int fac_1 = gcd(N, (int) pow(a, r/2) - 1);
            int fac_2 = gcd(N, (int) pow(a, r/2) + 1);
            printf("Found factors %d and %d for %d\n", fac_1, fac_2, N);
            
            return 0;
        }
    }

}
