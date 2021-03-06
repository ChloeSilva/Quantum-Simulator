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

void test_initialise_graph()
{
    printf("Testsing initialise_graph: ");

    ZXGraph *graph;
    Node *node;
    
    graph = initialise_graph(2);

    // test graph
    assert(graph->num_qubits == 2);
    assert(graph->num_nodes == 4);

    // test first input node
    node = get_node(graph->inputs[0], graph);
    assert(node->id == 0);
    assert(node->edge_count == 1);
    assert(node->edges[0] == 1);
    assert(node->type == INPUT);

    // test second input node
    node = get_node(graph->inputs[1], graph);
    assert(node->id == 2);
    assert(node->edge_count == 1);
    assert(node->edges[0] == 3);
    assert(node->type == INPUT);

    // test first output node
    node = get_node(graph->outputs[0], graph);
    assert(node->id == 1);
    assert(node->edge_count == 1);
    assert(node->edges[0] == 0);
    assert(node->type == OUTPUT);

    // test second output node
    node = get_node(graph->outputs[1], graph);
    assert(node->id == 3);
    assert(node->edge_count == 1);
    assert(node->edges[0] == 2);
    assert(node->type == OUTPUT);

    free_graph(graph);
    
    printf("Pass\n");
}

void test_initialise_input()
{
    printf("Testsing initialise_input: ");

    Node *input;

    input = initialise_input();

    // test input node
    assert(input->edge_count == 0);
    assert(input->type == INPUT);

    free(input);

    printf("Pass\n");
}

void test_initialise_output()
{
    printf("Testsing initialise_output: ");

    Node *output;

    output = initialise_output();

    // test input node
    assert(output->edge_count == 0);
    assert(output->type == OUTPUT);

    free(output);

    printf("Pass\n");
}

void test_initialise_hadamard()
{
    printf("Testsing initialise_hadamard: ");

    ZXGraph *graph;
    Node *hadamard;

    graph = initialise_graph(1);
    hadamard = initialise_hadamard(graph);

    // test hadamard
    assert(hadamard->id == 2);
    assert(hadamard->edge_count == 0);
    assert(hadamard->type == HADAMARD_BOX);

    // test graph
    assert(graph->num_qubits == 1);
    assert(graph->num_nodes == 3);
    assert(get_node(2, graph) == hadamard);

    free_graph(graph);

    printf("Pass\n");
}

void test_initialise_spider()
{
    printf("Testsing initialise_spider: ");

    ZXGraph *graph;
    Node *spider;

    graph = initialise_graph(1);
    spider = initialise_spider(RED, 1.5, graph);

    // test spider
    assert(spider->id == 2);
    assert(spider->edge_count == 0);
    assert(spider->type == SPIDER);
    assert(spider->color == RED);
    assert(spider->phase == 1.5);

    // test graph
    assert(graph->num_qubits == 1);
    assert(graph->num_nodes == 3);
    assert(get_node(2, graph) == spider);

    free_graph(graph);

    printf("Pass\n");
}

void test_get_node()
{
    printf("Testsing get_node: ");

    ZXGraph *graph;
    Node *node_0;
    Node *node_1;

    graph = initialise_graph(1);
    node_0 = get_node(0, graph);
    node_1 = get_node(1, graph);

    // test node
    assert(node_0->id == 0);
    assert(node_1->id == 1);

    free_graph(graph);

    printf("Pass\n");
}

void test_change_color()
{
    printf("Testing change_color: ");

    ZXGraph *graph = initialise_graph(1);
    Node *red_spider = initialise_spider(RED, 1.2f, graph);
    Node *green_spider = initialise_spider(GREEN, 1.2f, graph);

    change_color(red_spider);
    change_color(green_spider);

    // test red spider
    assert(red_spider->color == GREEN);
    assert(red_spider->phase == 1.2f);

    // test green spider
    assert(green_spider->color == RED);
    assert(green_spider->phase == 1.2f);

    free_graph(graph);

    printf("Pass\n");
}

void test_change_phase()
{
    printf("Testing change_phase: ");

    ZXGraph *graph = initialise_graph(1);
    Node *spider = initialise_spider(RED, 1.2f, graph);

    change_phase(spider, 0.8f);

    // test spider
    assert(spider->color == RED);
    assert(spider->phase == 0.8f);

    free_graph(graph);

    printf("Pass\n");
}

void test_add_edge()
{
    printf("Testsing add_edge: ");

    ZXGraph *graph;
    Node *node;
    Node *input;

    graph = initialise_graph(1);
    node = initialise_hadamard(graph);
    input = get_node(0, graph);
    add_edge(node, input);

    // test node is connected to input
    assert(node->edge_count == 1);
    assert(node->edges[0] == input->id);

    // test input is connected to node
    assert(input->edge_count == 2);
    assert(input->edges[1] == node->id);

    free_graph(graph);

    printf("Pass\n");
}

void test_remove_edge()
{
    printf("Testsing remove_edge: ");

    ZXGraph *graph;
    Node *input, *output, *node;

    graph = initialise_graph(1);
    node = initialise_hadamard(graph);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    add_edge(node, input);
    add_edge(node, output);
    remove_edge(input, output);

    // test input's edges
    assert(input->edge_count == 1);
    assert(input->edges[0] == node->id);

    // test output's edges
    assert(output->edge_count == 1);
    assert(output->edges[0] == node->id);

    // test node's edges
    assert(node->edge_count == 2);
    assert(node->edges[0] == input->id);
    assert(node->edges[1] == output->id);

    free_graph(graph);

    printf("Pass\n");
}

void test_remove_node()
{
    printf("Testsing remove_node: ");

    ZXGraph *graph = initialise_graph(1);
    Node *node = initialise_hadamard(graph);
    Node *input = get_node(graph->inputs[0], graph);
    Node *output = get_node(graph->outputs[0], graph);
    insert_node(node, input, output);
    remove_node(node, graph);

    // test graph
    assert(graph->num_qubits == 1);
    assert(graph->num_nodes == 2);
    assert(graph->id_counter == 3);

    // test input node
    assert(input->id == 0);
    assert(input->edge_count == 0);
    assert(input->type == INPUT);

    // test output node
    assert(output->id == 1);
    assert(output->edge_count == 0);
    assert(output->type == OUTPUT);

    free_graph(graph);

    printf("Pass\n");
}

void test_insert_node()
{
    printf("Testsing insert_node: ");

    ZXGraph *graph;
    Node *node, *input, *output;

    graph = initialise_graph(1);
    node = initialise_hadamard(graph);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    insert_node(node, input, output);

    // test input's edges
    assert(input->edge_count == 1);
    assert(input->edges[0] == node->id);

    // test output's edges
    assert(output->edge_count == 1);
    assert(output->edges[0] == node->id);

    // test node's edges
    assert(node->edge_count == 2);
    assert(node->edges[0] == input->id);
    assert(node->edges[1] == output->id);

    free_graph(graph);

    printf("Pass\n");
}

void test_is_connected()
{
    printf("Testing is_connected: ");

    ZXGraph *graph = initialise_graph(1);
    Node *input = get_node(graph->inputs[0], graph);
    Node *output = get_node(graph->outputs[0], graph);
    Node *node = initialise_hadamard(graph);
    insert_node(node, input, output);

    // test connected edges
    assert(is_connected(input, node));
    assert(is_connected(output, node));
    assert(!is_connected(input, output));

    free_graph(graph);

    printf("Pass\n");
}

void test_is_connected_io()
{
    printf("Testing is_connected_io: ");

    ZXGraph *graph = initialise_graph(1);
    Node *input = get_node(graph->inputs[0], graph);
    Node *output = get_node(graph->outputs[0], graph);
    Node *node_1 = initialise_hadamard(graph);
    Node *node_2 = initialise_hadamard(graph);
    Node *node_3 = initialise_hadamard(graph);
    insert_node(node_1, input, output);
    insert_node(node_2, node_1, output);
    insert_node(node_3, node_2, output);

    // test connected edges
    assert(is_connected_io(node_1, graph));
    assert(!is_connected_io(node_2, graph));
    assert(is_connected_io(node_3, graph));

    free_graph(graph);

    printf("Pass\n");
}

void test_is_proper_clifford()
{
    printf("Testing is_proper_clifford: ");

    ZXGraph *graph = initialise_graph(0);
    Node *pauli = initialise_spider(GREEN, 1.0, graph);
    Node *clifford = initialise_spider(GREEN, 1.5, graph);
    Node *t= initialise_spider(GREEN, 0.25, graph);

    assert(is_proper_clifford(clifford));
    assert(!is_proper_clifford(pauli));
    assert(!is_proper_clifford(t));

    free_graph(graph);

    printf("Pass\n");
}

void test_is_pauli()
{
    printf("Testing is_pauli: ");

    ZXGraph *graph = initialise_graph(0);
    Node *pauli = initialise_spider(GREEN, 1.0, graph);
    Node *clifford = initialise_spider(GREEN, 1.5, graph);
    Node *t= initialise_spider(GREEN, 0.25, graph);

    assert(is_pauli(pauli));
    assert(!is_pauli(clifford));
    assert(!is_pauli(t));

    free_graph(graph);

    printf("Pass\n");
}

void test_get_hadamard_edge()
{
    printf("Testing get_hadamard_edge: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0, graph);
    Node *hadamard = initialise_hadamard(graph);
    insert_node(spider_0, input_0, output_0);
    insert_node(spider_1, input_1, output_1);
    add_edge(hadamard, spider_0);
    add_edge(hadamard, spider_1);

    // when
    Node *test_hadamard = get_hadamard_edge(spider_0, spider_1, graph);

    // then
    assert(test_hadamard == hadamard);

    free_graph(graph);

    printf("Pass\n");
}

void test_get_hadamard_edge_spiders()
{
    printf("Testing get_hadamard_edge_spiders: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0, graph);
    Node *spider_2 = initialise_spider(GREEN, 0, graph);
    Node *spider_3 = initialise_spider(GREEN, 0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    insert_node(spider_0, input_0, output_0);
    insert_node(hadamard_0, spider_0, output_0);
    insert_node(spider_1, hadamard_0, output_0);
    insert_node(hadamard_1, spider_1, output_0);
    insert_node(spider_2, hadamard_1, output_0);
    insert_node(spider_3, input_1, output_1);
    add_edge(spider_1, spider_3);

    // when
    Node **spiders = get_hadamard_edge_spiders(spider_1, graph);

    // then
    assert(spiders[0] == spider_0);
    assert(spiders[1] == spider_2);
    assert(spiders[2] == NULL);

    free(spiders);
    free_graph(graph);

    printf("Pass\n");
}

int main()
{
    printf("\033[1;32m");

    test_initialise_graph();
    test_initialise_input();
    test_initialise_output();
    test_initialise_hadamard();
    test_initialise_spider();
    test_get_node();
    test_change_color();
    test_change_phase();
    test_add_edge();
    test_remove_edge();
    test_remove_node();
    test_insert_node();
    test_is_connected();
    test_is_connected_io();
    test_is_proper_clifford();
    test_is_pauli();
    test_get_hadamard_edge();
    test_get_hadamard_edge_spiders();

    printf("\033[0m");
}
