#include "circuit_synthesis.h"
#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>

void assert(int status)
{
    if(status == 1)
        return;

    printf("\033[1;31mFailed\n \033[0m");
    exit(EXIT_FAILURE);
}

void test_reverse()
{
    printf("Testing reverse: ");

    // given
    int numbers[10] = {0,1,2,3,4,5,6,7,8,9};

    // when
    flip_control_target(numbers, 10);
    reverse(numbers, 10);

    // then
    int test_numbers[10] = {9,8,7,6,5,4,3,2,1,0};

    for(int i=0; i<10; i++)
        assert(numbers[i] == test_numbers[i]);

    printf("Pass\n");
}

void test_transpose()
{
    printf("Testing transpose: ");

    // given
    int matrix[6][6] = {{1,1,0,0,0,0},
                        {1,0,0,1,1,0},
                        {0,1,0,0,1,0},
                        {1,1,1,1,1,1},
                        {1,1,0,1,1,1},
                        {0,0,1,1,1,0}};

    // when
    transpose((int *) matrix, 6);

    // then
    int test_matrix[6][6] = {{1,1,0,1,1,0},
                             {1,0,1,1,1,0},
                             {0,0,0,1,0,1},
                             {0,1,0,1,1,1},
                             {0,1,1,1,1,1},
                             {0,0,0,1,1,0}};

    for(int i=0; i<6; i++)
        for(int j=0; j<6; j++)
            assert(matrix[i][j] == test_matrix[i][j]);

    printf("Pass\n");
}

void test_bin_to_int()
{
    printf("Testing bin_to_int: ");

    // given
    int seven[3] = {1,1,1};
    int nineteen[5] = {1,0,0,1,1};

    // when
    int seven_result = bin_to_int(seven, 3);
    int nineteen_result = bin_to_int(nineteen, 5);

    // then
    assert(seven_result == 7);
    assert(nineteen_result == 19);

    printf("Pass\n");
}

void test_set_binary()
{
    printf("Testing set_binary: ");

    // given
    int x = 6;
    const int x_size = 6;
    int x_array[x_size];
    
    int y = 27;
    const int y_size = 5;
    int y_array[y_size];

    // when
    set_binary(x, x_size, x_array);
    set_binary(y, y_size, y_array);

    // then
    int x_array_test[x_size] = {0,1,1,0,0,0};
    int y_array_test[y_size] = {1,1,0,1,1};

    for(int i=0; i<x_size; i++)
        assert(x_array[i] == x_array_test[i]);

    for(int i=0; i<y_size; i++)
        assert(y_array[i] == y_array_test[i]);

    printf("Pass\n");
}

void test_sum()
{
    printf("Testing sum: ");

    // given
    const int x_size = 6;
    int x_array[x_size] = {0,1,1,0,0,0};
    
    const int y_size = 5;
    int y_array[y_size] = {1,1,0,1,1};

    // when
    int x = get_sum(x_array, x_size);
    int y = get_sum(y_array, y_size);

    // then
    assert(x == 2);
    assert(y == 4);

    printf("Pass\n");
}

void test_synthesise_linear_circuit()
{
    printf("Testing synthesise_linear_circuit: ");

    // given
    int cnot_layer_size;
    int matrix[6][6] = {{1,1,0,0,0,0},
                        {1,0,0,1,1,0},
                        {0,1,0,0,1,0},
                        {1,1,1,1,1,1},
                        {1,1,0,1,1,1},
                        {0,0,1,1,1,0}};

    // when
    int *circuit = synthesise_linear_circuit((int *) matrix, 6, &cnot_layer_size);

    // then
    int test_circuit[30] = {4,3,1,0,3,1,5,2,4,2,4,3,5,4,2,
                            3,3,2,3,5,2,4,1,2,0,1,0,4,0,3};
    
    for(int i=0; i<30; i++)
        assert(circuit[i] == test_circuit[i]);

    assert(cnot_layer_size == 30);

    free(circuit);

    printf("Pass\n");
}

void test_get_biadjacency_matrix()
{
    printf("Testing get_biadjacency_matrix: ");

    // given
    ZXGraph *graph = initialise_graph(4);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *input_2 = get_node(graph->inputs[2], graph);
    Node *input_3 = get_node(graph->inputs[3], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *output_2 = get_node(graph->outputs[2], graph);
    Node *output_3 = get_node(graph->outputs[3], graph);
    Node *left_spider_0 = initialise_spider(GREEN, 0.0, graph);
    Node *left_spider_1 = initialise_spider(GREEN, 0.0, graph);
    Node *left_spider_2 = initialise_spider(GREEN, 0.0, graph);
    Node *left_spider_3 = initialise_spider(GREEN, 0.0, graph);
    Node *right_spider_0 = initialise_spider(GREEN, 0.0, graph);
    Node *right_spider_1 = initialise_spider(GREEN, 0.0, graph);
    Node *right_spider_2 = initialise_spider(GREEN, 0.0, graph);
    Node *right_spider_3 = initialise_spider(GREEN, 0.0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    Node *hadamard_3 = initialise_hadamard(graph);
    Node *hadamard_4 = initialise_hadamard(graph);
    Node *hadamard_5 = initialise_hadamard(graph);
    Node *hadamard_6 = initialise_hadamard(graph);
    Node *hadamard_7 = initialise_hadamard(graph);
    Node *hadamard_8 = initialise_hadamard(graph);

    insert_node(left_spider_0, input_0, output_0);
    insert_node(right_spider_0, left_spider_0, output_0);
    insert_node(left_spider_1, input_1, output_1);
    insert_node(right_spider_1, left_spider_1, output_1);
    insert_node(left_spider_2, input_2, output_2);
    insert_node(right_spider_2, left_spider_2, output_2);
    insert_node(left_spider_3, input_3, output_3);
    insert_node(right_spider_3, left_spider_3, output_3);
    
    insert_node(hadamard_0, left_spider_0, right_spider_0);
    add_edge(left_spider_0, right_spider_2);
    insert_node(hadamard_1, left_spider_0, right_spider_2);
    add_edge(left_spider_1, right_spider_2);
    insert_node(hadamard_2, left_spider_1, right_spider_2);
    add_edge(left_spider_2, right_spider_0);
    insert_node(hadamard_3, left_spider_2, right_spider_0);
    add_edge(left_spider_2, right_spider_1);
    insert_node(hadamard_4, left_spider_2, right_spider_1);
    insert_node(hadamard_5, left_spider_2, right_spider_2);
    add_edge(left_spider_3, right_spider_0);
    insert_node(hadamard_6, left_spider_3, right_spider_0);
    add_edge(left_spider_3, right_spider_1);
    insert_node(hadamard_7, left_spider_3, right_spider_1);
    insert_node(hadamard_8, left_spider_3, right_spider_3);

    // when
    int *matrix = get_biadjacency_matrix(graph);

    // then
    int test_matrix[16] = {1, 0, 1, 0,
                           0, 0, 1, 0,
                           1, 1, 1, 0,
                           1, 1, 0, 1};

    for(int i=0; i<16; i++)
            assert(matrix[i] == test_matrix[i]);

    free(matrix);
    free_graph(graph);

    printf("Pass\n");
}

void test_synthesise_multi_cnot()
{
    printf("Testing synthesise_multi_cnot: ");

    // given
    int n = 3;

    // when
    int *circuit = synthesise_multi_cnot(n);

    // then
    int circuit_test[51] = {0,3,0,
                            1,3,0,
                            0,1,1,
                            1,3,1,
                            0,1,1,
                            2,3,0,
                            0,2,1,
                            2,3,1,
                            0,2,1,
                            1,2,1,
                            2,3,1,
                            1,2,1,
                            0,2,0,
                            1,2,0,
                            2,3,0,
                            0,2,0,
                            1,2,0};

    for(int i=0; circuit[i] != n+1; i++)
        assert(circuit[i] == circuit_test[i]);

    free(circuit);

    printf("Pass\n");
}

int main()
{
    printf("\033[1;32m");

    test_reverse();
    test_transpose();
    test_bin_to_int();
    test_set_binary();
    test_sum();
    test_synthesise_linear_circuit();
    test_get_biadjacency_matrix();
    test_synthesise_multi_cnot();

    printf("\033[0m");
}
