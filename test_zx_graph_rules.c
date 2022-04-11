#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_apply_fusion()
{
    printf("Testing apply_fusion: ");

    ZXGraph *graph = initialise_graph(2);
    Node *input_1 = get_node(graph->inputs[0], graph);
    Node *input_2 = get_node(graph->inputs[1], graph);
    Node *output_1 = get_node(graph->outputs[0], graph);
    Node *output_2 = get_node(graph->outputs[1], graph);
    Node *spider_1 = initialise_spider(RED, 0.8f, graph);
    Node *spider_2 = initialise_spider(RED, 1.3f, graph);
    insert_node(spider_1, input_1, output_1);
    insert_node(spider_2, input_2, output_2);
    add_edge(spider_1, spider_2);
    apply_fusion(spider_1, spider_2, graph);

    // test graph
    assert(graph->num_qubits == 2);
    assert(graph->num_nodes == 5);
    assert(graph->id_counter == 6);

    // test spider
    assert(spider_1->id == 4);
    assert(spider_1->edge_count == 4);
    assert(spider_1->edges[0] == input_1->id);
    assert(spider_1->edges[1] == output_1->id);
    assert(spider_1->edges[2] == input_2->id);
    assert(spider_1->edges[3] == output_2->id);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == RED);
    assert(spider_1->phase == 2.1f);

    // test inputs
    assert(input_1->id == 0);
    assert(input_1->edge_count == 1);
    assert(input_1->edges[0] == spider_1->id);
    assert(input_1->type == INPUT);
    assert(input_2->id == 2);
    assert(input_2->edge_count == 1);
    assert(input_2->edges[0] == spider_1->id);
    assert(input_2->type == INPUT);

    // test outputs
    assert(output_1->id == 1);
    assert(output_1->edge_count == 1);
    assert(output_1->edges[0] == spider_1->id);
    assert(output_1->type == OUTPUT);
    assert(output_2->id == 3);
    assert(output_2->edge_count == 1);
    assert(output_2->edges[0] == spider_1->id);
    assert(output_2->type == OUTPUT);

    printf("Pass\n");
}

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
    test_apply_fusion();
    test_apply_color_change();
    test_apply_id1();
    test_apply_id2();
}
