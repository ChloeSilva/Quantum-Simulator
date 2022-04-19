#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>

void assert(int status)
{
    if(status == 1)
        return;

    printf("\033[1;31mFailed\n \033[0m");
    exit(EXIT_FAILURE);
}

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

    // given
    ZXGraph *graph = initialise_graph(1);
    Node *input = get_node(graph->inputs[0], graph);
    Node *output = get_node(graph->outputs[0], graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    insert_node(hadamard_1, input, output);
    insert_node(hadamard_2, hadamard_1, output);

    // when
    apply_id2(hadamard_1, hadamard_2, graph);

    //then
    // test graph
    assert(graph->num_nodes == 2);

    // test input
    assert(input->edge_count == 1);
    assert(is_connected(input, output));
    assert(input->type == INPUT);
    
    // test output
    assert(output->edge_count == 1);
    assert(is_connected(output, input));
    assert(output->type == OUTPUT);

    free_graph(graph);

    printf("Pass\n");
}

void test_apply_local_complement()
{
    printf("Testing apply_local_complement: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 0.5, graph);
    Node *spider_1 = initialise_spider(GREEN, 0.0, graph);
    Node *spider_2 = initialise_spider(GREEN, 0.5, graph);
    Node *spider_3 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_4 = initialise_spider(GREEN, 1.5, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    Node *hadamard_3 = initialise_hadamard(graph);
    Node *hadamard_4 = initialise_hadamard(graph);
    Node *hadamard_5 = initialise_hadamard(graph);
    insert_node(spider_1, input_0, output_0);
    insert_node(hadamard_0, spider_1, output_0);
    insert_node(spider_2, hadamard_0, output_0);
    insert_node(spider_3, input_1, output_1);
    insert_node(hadamard_1, spider_3, output_1);
    insert_node(spider_4, hadamard_1, output_1);
    add_edge(hadamard_2, spider_1);
    add_edge(hadamard_2, spider_0);
    add_edge(hadamard_3, spider_2);
    add_edge(hadamard_3, spider_0);
    add_edge(hadamard_4, spider_3);
    add_edge(hadamard_4, spider_0);
    add_edge(hadamard_5, spider_4);
    add_edge(hadamard_5, spider_0);
    
    // when
    apply_local_complement(spider_0, graph);

    //then
    Node *hadamard_6 = graph->nodes[8];
    Node *hadamard_7 = graph->nodes[9];
    Node *hadamard_8 = graph->nodes[10];
    Node *hadamard_9 = graph->nodes[11];

    // test graph
    assert(graph->num_nodes == 12);

    // test input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, spider_1));

    // test input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider_3));
    
    // test output 0 
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, spider_2));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_4));

    // test spider 1
    assert(spider_1->edge_count == 3);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == -0.5);
    assert(is_connected(spider_1, input_0));
    assert(is_connected(spider_1, hadamard_6));
    assert(is_connected(spider_1, hadamard_7));

    // test spider 2
    assert(spider_2->edge_count == 3);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == 0.0);
    assert(is_connected(spider_2, output_0));
    assert(is_connected(spider_2, hadamard_8));
    assert(is_connected(spider_2, hadamard_9));

    // test spider 3
    assert(spider_3->edge_count == 3);
    assert(spider_3->type == SPIDER);
    assert(spider_3->color == GREEN);
    assert(spider_3->phase == 0.5);
    assert(is_connected(spider_3, input_1));
    assert(is_connected(spider_3, hadamard_6));
    assert(is_connected(spider_3, hadamard_8));

    // test spider 4
    assert(spider_4->edge_count == 3);
    assert(spider_4->type == SPIDER);
    assert(spider_4->color == GREEN);
    assert(spider_4->phase == 1.0);
    assert(is_connected(spider_4, output_1));
    assert(is_connected(spider_4, hadamard_9));
    assert(is_connected(spider_4, hadamard_7));

    // test hadamard 6
    assert(hadamard_6->edge_count == 2);
    assert(hadamard_6->type == HADAMARD_BOX);
    assert(is_connected(hadamard_6, spider_1));
    assert(is_connected(hadamard_6, spider_3));

    // test hadamard 7
    assert(hadamard_7->edge_count == 2);
    assert(hadamard_7->type == HADAMARD_BOX);
    assert(is_connected(hadamard_7, spider_1));
    assert(is_connected(hadamard_7, spider_4));

    // test hadamard 8
    assert(hadamard_8->edge_count == 2);
    assert(hadamard_8->type == HADAMARD_BOX);
    assert(is_connected(hadamard_8, spider_2));
    assert(is_connected(hadamard_8, spider_3));

    // test hadamard 9
    assert(hadamard_9->edge_count == 2);
    assert(hadamard_9->type == HADAMARD_BOX);
    assert(is_connected(hadamard_9, spider_2));
    assert(is_connected(hadamard_9, spider_4));

    free_graph(graph);

    printf("Pass\n");
}

void test_apply_pivot()
{
    printf("Testing apply_pivot: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_1 = initialise_spider(GREEN, 2.0, graph);
    Node *spider_2 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_3 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_4 = initialise_spider(GREEN, 0.0, graph);
    Node *spider_5 = initialise_spider(GREEN, 1.0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    Node *hadamard_3 = initialise_hadamard(graph);
    Node *hadamard_4 = initialise_hadamard(graph);
    Node *hadamard_5 = initialise_hadamard(graph);
    Node *hadamard_6 = initialise_hadamard(graph);
    Node *hadamard_7 = initialise_hadamard(graph);
    Node *hadamard_8 = initialise_hadamard(graph);
    Node *hadamard_9 = initialise_hadamard(graph);
    insert_node(spider_2, input_0, output_0);
    insert_node(hadamard_0, spider_2, output_0);
    insert_node(spider_0, hadamard_0, output_0);
    insert_node(hadamard_1, spider_0, output_0);
    insert_node(spider_1, hadamard_1, output_0);
    insert_node(hadamard_2, spider_1, output_0);
    insert_node(spider_3, hadamard_2, output_0);
    insert_node(spider_4, input_1, output_1);
    insert_node(hadamard_3, spider_4, output_1);
    insert_node(spider_5, hadamard_3, output_1);
    add_edge(hadamard_4, spider_2);
    add_edge(hadamard_4, spider_4);
    add_edge(hadamard_5, spider_0);
    add_edge(hadamard_5, spider_4);
    add_edge(hadamard_6, spider_1);
    add_edge(hadamard_6, spider_5);
    add_edge(hadamard_7, spider_3);
    add_edge(hadamard_7, spider_5);
    add_edge(hadamard_8, spider_4);
    add_edge(hadamard_8, spider_1);
    add_edge(hadamard_9, spider_5);
    add_edge(hadamard_9, spider_0);
    
    // when
    apply_pivot(spider_0, spider_1, graph);

    //then
    hadamard_0 = graph->nodes[8];
    hadamard_1 = graph->nodes[9];
    hadamard_2 = graph->nodes[10];
    hadamard_3 = graph->nodes[11];

    // test graph
    assert(graph->num_nodes == 12);

    // test input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, spider_2));

    // test input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider_4));
    
    // test output 0 
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, spider_3));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_5));

    // test spider 2
    assert(spider_2->edge_count == 3);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == 3.0);
    assert(is_connected(spider_2, input_0));
    assert(is_connected(spider_2, hadamard_0));
    assert(is_connected(spider_2, hadamard_1));

    // test spider 3
    assert(spider_3->edge_count == 3);
    assert(spider_3->type == SPIDER);
    assert(spider_3->color == GREEN);
    assert(spider_3->phase == 2.0);
    assert(is_connected(spider_3, output_0));
    assert(is_connected(spider_3, hadamard_1));
    assert(is_connected(spider_3, hadamard_3));

    // test spider 4
    assert(spider_4->edge_count == 3);
    assert(spider_4->type == SPIDER);
    assert(spider_4->color == GREEN);
    assert(spider_4->phase == 4.0);
    assert(is_connected(spider_4, input_1));
    assert(is_connected(spider_4, hadamard_2));
    assert(is_connected(spider_4, hadamard_3));

    // test spider 5
    assert(spider_5->edge_count == 3);
    assert(spider_5->type == SPIDER);
    assert(spider_5->color == GREEN);
    assert(spider_5->phase == 5.0);
    assert(is_connected(spider_5, output_1));
    assert(is_connected(spider_5, hadamard_2));
    assert(is_connected(spider_5, hadamard_0));

    // test hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, spider_2));
    assert(is_connected(hadamard_0, spider_5));

    // test hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, spider_2));
    assert(is_connected(hadamard_1, spider_3));

    // test hadamard 2
    assert(hadamard_2->edge_count == 2);
    assert(hadamard_2->type == HADAMARD_BOX);
    assert(is_connected(hadamard_2, spider_4));
    assert(is_connected(hadamard_2, spider_5));

    // test hadamard 3
    assert(hadamard_3->edge_count == 2);
    assert(hadamard_3->type == HADAMARD_BOX);
    assert(is_connected(hadamard_3, spider_3));
    assert(is_connected(hadamard_3, spider_4));

    free_graph(graph);

    printf("Pass\n");
}

void test_extract_boundary()
{
    printf("Testing extract_boundary: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0.0, graph);
    insert_node(spider_0, input_0, output_0);
    insert_node(spider_1, input_1, output_1);
    add_edge(spider_0, spider_1);

    // when
    extract_boundary(spider_0, graph);

    // then
    Node *hadamard_0 = graph->nodes[6];
    Node *spider_2 = graph->nodes[7];
    Node *hadamard_1 = graph->nodes[8];
    Node *spider_3 = graph->nodes[9];

    // test graph
    assert(graph->num_nodes == 10);

    // test input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, spider_0));

    // test input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider_1));

    // test output 0
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, spider_3));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_1));

    // test spider 0
    assert(spider_0->edge_count == 3);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == GREEN);
    assert(spider_0->phase == 0.0);
    assert(is_connected(spider_0, input_0));
    assert(is_connected(spider_0, hadamard_0));
    assert(is_connected(spider_0, spider_1));

    // test spider 1
    assert(spider_1->edge_count == 3);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == 0.0);
    assert(is_connected(spider_1, input_1));
    assert(is_connected(spider_1, output_1));
    assert(is_connected(spider_1, spider_0));

    // test spider 2
    assert(spider_2->edge_count == 2);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == 0.0);
    assert(is_connected(spider_2, hadamard_0));
    assert(is_connected(spider_2, hadamard_1));

    // test spider 3
    assert(spider_3->edge_count == 2);
    assert(spider_3->type == SPIDER);
    assert(spider_3->color == GREEN);
    assert(spider_3->phase == 1.0);
    assert(is_connected(spider_3, hadamard_1));
    assert(is_connected(spider_3, output_0));

    // test hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, spider_0));
    assert(is_connected(hadamard_0, spider_2));

    // test hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, spider_2));
    assert(is_connected(hadamard_1, spider_3));

    free_graph(graph);

    printf("Pass\n");
}

int main()
{
    printf("\033[1;32m");

    test_apply_fusion();
    test_apply_color_change();
    test_apply_id1();
    test_apply_id2();
    test_apply_local_complement();
    test_apply_pivot();
    test_extract_boundary();

    printf("\033[0m");
}
