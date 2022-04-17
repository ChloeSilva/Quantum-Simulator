#include "circuit_synthesis.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_synthesise_linear_circuit()
{
    printf("Testing synthesise_linear_circuit: ");

    // given
    int matrix[6][6] = {{1,1,0,0,0,0},
                        {1,0,0,1,1,0},
                        {0,1,0,0,1,0},
                        {1,1,1,1,1,1},
                        {1,1,0,1,1,1},
                        {0,0,1,1,1,0}};

    // when
    int *circuit = synthesise_linear_circuit((int *) matrix, 6);

    // then
    int test_circuit[30] = {1,4,1,5,1,2,2,3,3,5,4,6,4,3,3,4,6,5,5,4,5,3,6,3,4,2,2,1,5,4};
    
    for(int i=0; i<30; i++)
        assert(circuit[i] == test_circuit[i]-1);

    free(circuit);

    printf("Pass\n");
}

int main()
{
    test_synthesise_linear_circuit();
}
