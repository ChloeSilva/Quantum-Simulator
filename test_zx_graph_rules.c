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
    assert(is_connected(spider_1, input_1));
    assert(is_connected(spider_1, output_1));
    assert(is_connected(spider_1, input_2));
    assert(is_connected(spider_1, output_2));
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == RED);
    assert(spider_1->phase == 2.1f);

    // test inputs
    assert(input_1->id == 0);
    assert(input_1->edge_count == 1);
    assert(is_connected(input_1, spider_1));
    assert(input_1->type == INPUT);
    assert(input_2->id == 2);
    assert(input_2->edge_count == 1);
    assert(is_connected(input_2, spider_1));
    assert(input_2->type == INPUT);

    // test outputs
    assert(output_1->id == 1);
    assert(output_1->edge_count == 1);
    assert(is_connected(output_1, spider_1));
    assert(output_1->type == OUTPUT);
    assert(output_2->id == 3);
    assert(output_2->edge_count == 1);
    assert(is_connected(output_2, spider_1));
    assert(output_2->type == OUTPUT);

    free_graph(graph);

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
    assert(is_connected(spider, hadamard_1));
    assert(is_connected(spider, hadamard_2));
    assert(spider->type == SPIDER);
    assert(spider->color == GREEN);
    assert(spider->phase == 1.2f);

    // test hadamard 1
    assert(hadamard_1->id == 3);
    assert(hadamard_1->edge_count == 2);
    assert(is_connected(hadamard_1, spider));
    assert(is_connected(hadamard_1, input));
    assert(hadamard_1->type == HADAMARD_BOX);

    // test hadamard 2
    assert(hadamard_2->id == 4);
    assert(hadamard_2->edge_count == 2);
    assert(is_connected(hadamard_2, spider));
    assert(is_connected(hadamard_2, output));
    assert(hadamard_2->type == HADAMARD_BOX);

    // test input
    assert(input->edge_count == 1);
    assert(is_connected(input, hadamard_1));
    assert(input->type == INPUT);

    // test output
    assert(output->edge_count == 1);
    assert(is_connected(output, hadamard_2));
    assert(output->type == OUTPUT);

    free_graph(graph);

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
    assert(is_connected(spider, input));
    assert(is_connected(spider, output));
    assert(spider->type == SPIDER);
    assert(spider->color == RED);
    assert(spider->phase == 0.0);

    // test input
    assert(input->edge_count == 1);
    assert(is_connected(input, spider));
    assert(input->type == INPUT);
    
    // test output
    assert(output->edge_count == 1);
    assert(is_connected(output, spider));
    assert(output->type == OUTPUT);

    free_graph(graph);

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
    assert(is_connected(hadamard_1, input));
    assert(is_connected(hadamard_1, hadamard_2));
    assert(hadamard_1->type == HADAMARD_BOX);

    // test hadamard 2
    assert(hadamard_2->id == 3);
    assert(hadamard_2->edge_count == 2);
    assert(is_connected(hadamard_2, hadamard_1));
    assert(is_connected(hadamard_2, output));
    assert(hadamard_2->type == HADAMARD_BOX);

    // test input
    assert(input->edge_count == 1);
    assert(is_connected(input, hadamard_1));
    assert(input->type == INPUT);
    
    // test output
    assert(output->edge_count == 1);
    assert(is_connected(output, hadamard_2));
    assert(output->type == OUTPUT);

    free_graph(graph);

    printf("Pass\n");
}

int main()
{
    test_apply_fusion();
    test_apply_color_change();
    test_apply_id1();
    test_apply_id2();
}
