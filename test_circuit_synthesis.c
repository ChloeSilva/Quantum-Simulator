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
    int test_circuit[30] = {1,4,1,5,1,2,2,3,3,5,4,6,4,3,3,4,
                            6,5,5,4,5,3,6,3,4,2,2,1,5,4};
    
    for(int i=0; i<30; i++)
        assert(circuit[i] == test_circuit[i]-1);

    free(circuit);

    printf("Pass\n");
}

void test_get_biadjacency_matrix()
{
    printf("Testing synthesise_linear_circuit: ");

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

int main()
{
    printf("\033[1;32m");

    test_synthesise_linear_circuit();
    test_get_biadjacency_matrix();

    printf("\033[0m");
}
