#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_initialise_graph()
{
    ZXGraph *graph;
    Node *node;

    graph = initialise_graph(2);

    // test graph
    assert(graph->qubit_count == 2);
    assert(graph->node_count == 4);

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
}

void test_initialise_input()
{
    Node *input;

    input = initialise_input();

    // test input node
    assert(input->edge_count == 0);
    assert(input->type == INPUT);

    free(input);
}

void test_initialise_output()
{
    Node *output;

    output = initialise_output();

    // test input node
    assert(output->edge_count == 0);
    assert(output->type == OUTPUT);

    free(output);
}

void test_initialise_hadamard()
{
    ZXGraph *graph;
    Node *hadamard;

    graph = initialise_graph(1);
    hadamard = initialise_hadamard(graph);

    // test hadamard
    assert(hadamard->id == 2);
    assert(hadamard->edge_count == 0);
    assert(hadamard->type == HADAMARD);

    // test graph
    assert(graph->qubit_count == 1);
    assert(graph->node_count = 3);
    assert(get_node(2, graph) == hadamard);

    free_graph(graph);
}

void test_initialise_spider()
{
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
    assert(graph->qubit_count == 1);
    assert(graph->node_count == 3);
    assert(get_node(2, graph) == spider);

    free_graph(graph);
}

void test_get_node()
{
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
}

void test_add_edge()
{
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
}

void test_remove_edge()
{
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
}

void test_insert_node()
{
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
}

int main()
{
    test_initialise_graph();
    test_initialise_input();
    test_initialise_output();
    test_initialise_hadamard();
    test_initialise_spider();
    test_get_node();
    test_add_edge();
    test_remove_edge();
    test_insert_node();
}