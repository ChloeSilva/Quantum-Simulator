#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_apply_color_change()
{
    printf("Testing apply_color_change: ");

    ZXGraph *graph;
    Node *input, *output, *spider, *hadamard_1, *hadamard_2;

    graph = initialise_graph(1);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    spider = initialise_spider(RED, 1.2f, graph);
    insert_node(spider, input, output);
    apply_color_change(spider, graph);

    hadamard_1 = get_node(3, graph);
    hadamard_2 = get_node(4, graph);

    // test spider
    assert(spider->id == 2);
    assert(spider->edge_count == 2);
    assert(spider->edges[0] == hadamard_1->id);
    assert(spider->edges[1] == hadamard_2->id);
    assert(spider->type == SPIDER);
    assert(spider->color == GREEN);
    assert(spider->phase == 1.2f);

    // test hadamard 1
    assert(hadamard_1->id == 3);
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->edges[0] == spider->id);
    assert(hadamard_1->edges[1] == input->id);
    assert(hadamard_1->type == HADAMARD);

    // test hadamard 2
    assert(hadamard_2->id == 4);
    assert(hadamard_2->edge_count == 2);
    assert(hadamard_2->edges[0] == spider->id);
    assert(hadamard_2->edges[1] == output->id);
    assert(hadamard_2->type == HADAMARD);

    // test input
    assert(input->edge_count == 1);
    assert(input->edges[0] == hadamard_1->id);
    assert(input->type == INPUT);

    // test output
    assert(output->edge_count == 1);
    assert(output->edges[0] == hadamard_2->id);
    assert(output->type == OUTPUT);

    printf("Pass\n");
}

void test_apply_id1()
{
    printf("Testing apply_id1: ");

    ZXGraph *graph;
    Node *input, *output, *spider;

    graph = initialise_graph(1);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    apply_id1(input, output, RED, graph);

    spider = get_node(2, graph);

    // test spider
    assert(spider->id == 2);
    assert(spider->edge_count == 2);
    assert(spider->edges[0] == input->id);
    assert(spider->edges[1] == output->id);
    assert(spider->type == SPIDER);
    assert(spider->color == RED);
    assert(spider->phase == 0.0);

    // test input
    assert(input->edge_count == 1);
    assert(input->edges[0] == spider->id);
    assert(input->type == INPUT);
    
    // test output
    assert(output->edge_count == 1);
    assert(output->edges[0] == spider->id);
    assert(output->type == OUTPUT);

    printf("Pass\n");
}

void test_apply_id2()
{
    printf("Testing apply_id2: ");

    ZXGraph *graph;
    Node *input, *output, *hadamard_1, *hadamard_2;

    graph = initialise_graph(1);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    apply_id2(input, output, graph);

    hadamard_1 = get_node(2, graph);
    hadamard_2 = get_node(3, graph);

    // test hadamard 1
    assert(hadamard_1->id == 2);
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->edges[0] == input->id);
    assert(hadamard_1->edges[1] == hadamard_2->id);
    assert(hadamard_1->type == HADAMARD);

    // test hadamard 2
    assert(hadamard_2->id == 3);
    assert(hadamard_2->edge_count == 2);
    assert(hadamard_2->edges[0] == hadamard_1->id);
    assert(hadamard_2->edges[1] == output->id);
    assert(hadamard_2->type == HADAMARD);

    // test input
    assert(input->edge_count == 1);
    assert(input->edges[0] == hadamard_1->id);
    assert(input->type == INPUT);
    
    // test output
    assert(output->edge_count == 1);
    assert(output->edges[0] == hadamard_2->id);
    assert(output->type == OUTPUT);

    printf("Pass\n");
}

int main()
{
    test_apply_color_change();
    test_apply_id1();
    test_apply_id2();
}
