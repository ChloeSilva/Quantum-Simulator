#include "simplify.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

void test_circuit_to_zx_graph()
{
    // TODO: include diagrams of unit tests 
    // with code and explanation in appendix
    // email Thorsten to ask how much I should
    // write about implementation decisions
    // ie. data structures, testing, functions...
    printf("Testing circuit_to_zx_graph: ");

    // given
    Circuit *circuit = initialise_circuit(3);
    add_gate(HADAMARD, 0, circuit);
    add_gate(HADAMARD, 1, circuit);
    add_controlled_gate(NOT, 1, 2, circuit);
    add_gate(NOT, 2, circuit);

    // when
    ZXGraph *graph = circuit_to_zx_graph(circuit);

    // then
    Node *input_0 =  get_node(graph->inputs[0], graph);
    Node *input_1 =  get_node(graph->inputs[1], graph);
    Node *input_2 =  get_node(graph->inputs[2], graph);
    Node *output_0 =  get_node(graph->outputs[0], graph);
    Node *output_1 =  get_node(graph->outputs[1], graph);
    Node *output_2 =  get_node(graph->outputs[2], graph);
    Node *hadamard_0 = graph->nodes[6];
    Node *hadamard_1 = graph->nodes[7];
    Node *spider_0 = graph->nodes[8];
    Node *spider_1 = graph->nodes[9];
    Node *spider_2 = graph->nodes[10];

    // testing graph
    assert(graph->num_qubits == 3);
    assert(graph->num_nodes == 11);
    
    // testing input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, hadamard_0));

    // testing input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, hadamard_1));

    // testing input 2
    assert(input_2->edge_count == 1);
    assert(input_2->type == INPUT);
    assert(is_connected(input_2, spider_1));

    // testing output 0
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, hadamard_0));

    // testing output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_0));

    // testing output 2
    assert(output_2->edge_count == 1);
    assert(output_2->type == OUTPUT);
    assert(is_connected(output_2, spider_2));

    // testing hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, input_0));
    assert(is_connected(hadamard_0, output_0));

    // testing hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, input_1));
    assert(is_connected(hadamard_1, spider_0));

    // testing spider 0
    assert(spider_0->edge_count == 3);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == RED);
    assert(spider_0->phase == 0);
    assert(is_connected(spider_0, hadamard_1));
    assert(is_connected(spider_0, output_1));
    assert(is_connected(spider_0, spider_1));

    // testing spider 1
    assert(spider_1->edge_count == 3);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == 0);
    assert(is_connected(spider_1, input_2));
    assert(is_connected(spider_1, spider_0));
    assert(is_connected(spider_1, spider_2));

    // testing spider 2
    assert(spider_2->edge_count == 2);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == RED);
    assert(spider_2->phase == (float) M_PI);
    assert(is_connected(spider_2, spider_1));
    assert(is_connected(spider_2, output_2));

    free_circuit(circuit);
    free_graph(graph);

    printf("Pass\n");
}

void test_remove_z_spiders()
{
    printf("Testing remove_z_spiders: ");

    // given
    Circuit *circuit = initialise_circuit(2);
    add_controlled_gate(NOT, 0, 1, circuit);
    add_gate(NOT, 1, circuit);
    
    ZXGraph *graph = circuit_to_zx_graph(circuit);   

    // when
    remove_z_spiders(graph);

    // then
    Node *input_0 =  get_node(graph->inputs[0], graph);
    Node *input_1 =  get_node(graph->inputs[1], graph);
    Node *output_0 =  get_node(graph->outputs[0], graph);
    Node *output_1 =  get_node(graph->outputs[1], graph);
    Node *spider_0 = graph->nodes[4];
    Node *spider_1 = graph->nodes[5];
    Node *spider_2 = graph->nodes[6];
    Node *hadamard_0 = graph->nodes[7];
    Node *hadamard_1 = graph->nodes[8];
    Node *hadamard_2 = graph->nodes[9];
    Node *hadamard_3 = graph->nodes[10];
    Node *hadamard_4 = graph->nodes[11];

    // testing graph
    assert(graph->num_qubits == 2);
    assert(graph->num_nodes == 12);
    
    // testing input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, hadamard_0));

    // testing input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider_1));

    // testing output 0
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, hadamard_1));

    // testing output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, hadamard_4));

    // testing spider 0
    assert(spider_0->edge_count == 3);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == GREEN);
    assert(spider_0->phase == 0);
    assert(is_connected(spider_0, hadamard_0));
    assert(is_connected(spider_0, hadamard_1));
    assert(is_connected(spider_0, hadamard_2));

    // testing spider 1
    assert(spider_1->edge_count == 3);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == 0);
    assert(is_connected(spider_1, input_1));
    assert(is_connected(spider_1, hadamard_2));
    assert(is_connected(spider_1, hadamard_3));

    // testing spider 2
    assert(spider_2->edge_count == 2);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == (float) M_PI);
    assert(is_connected(spider_2, hadamard_3));
    assert(is_connected(spider_2, hadamard_4));

    // testing hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, input_0));
    assert(is_connected(hadamard_0, spider_0));

    // testing hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, spider_0));
    assert(is_connected(hadamard_1, output_0));

    // testing hadamard 2
    assert(hadamard_2->edge_count == 2);
    assert(hadamard_2->type == HADAMARD_BOX);
    assert(is_connected(hadamard_2, spider_0));
    assert(is_connected(hadamard_2, spider_1));

    // testing hadamard 3
    assert(hadamard_3->edge_count == 2);
    assert(hadamard_3->type == HADAMARD_BOX);
    assert(is_connected(hadamard_3, spider_1));
    assert(is_connected(hadamard_3, spider_2));

    // testing hadamard 4
    assert(hadamard_4->edge_count == 2);
    assert(hadamard_4->type == HADAMARD_BOX);
    assert(is_connected(hadamard_4, spider_2));
    assert(is_connected(hadamard_4, output_1));

    free_circuit(circuit);
    free_graph(graph);

    printf("Pass\n");
}

void test_add_hadamard_edges()
{
    printf("Testing add_hadamard_edges: ");

    // given
    Circuit *circuit = initialise_circuit(2);
    add_gate(HADAMARD, 0, circuit);
    add_gate(HADAMARD, 0, circuit);
    add_gate(NOT, 1, circuit);
    add_gate(NOT, 1, circuit);

    ZXGraph *graph = circuit_to_zx_graph(circuit);  

    // when
    add_hadamard_edges(graph);

    // then
    Node *input_0 =  get_node(graph->inputs[0], graph);
    Node *input_1 =  get_node(graph->inputs[1], graph);
    Node *output_0 =  get_node(graph->outputs[0], graph);
    Node *output_1 =  get_node(graph->outputs[1], graph);
    Node *spider = graph->nodes[4];

    // test graph
    assert(graph->num_qubits == 2);
    assert(graph->num_nodes == 5);

    // test input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, output_0));

    // test input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider));

    // test output 0
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, input_0));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider));

    // test spider
    assert(spider->edge_count == 2);
    assert(spider->type == SPIDER);
    assert(spider->color == RED);
    assert(spider->phase == (float) (M_PI * 2));
    assert(is_connected(spider, input_1));
    assert(is_connected(spider, output_1));

    free_circuit(circuit);
    free_graph(graph);

    printf("Pass\n");
}

void test_clean_edges()
{
    printf("Testing clean_edges: ");

    // given
    ZXGraph *graph = initialise_graph(2);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *spider_0 = initialise_spider(GREEN, 0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    insert_node(spider_0, input_0, output_0);
    insert_node(spider_1, input_1, output_1);
    add_edge(hadamard_0, spider_0);
    add_edge(hadamard_0, spider_1);
    add_edge(hadamard_1, spider_0);
    add_edge(hadamard_1, spider_1);
    add_edge(spider_0, spider_0);
    add_edge(hadamard_2, spider_1);
    add_edge(hadamard_2, spider_1);

    // when
    clean_edges(graph);

    // then
    // test graph
    assert(graph->num_qubits == 2);
    assert(graph->num_nodes == 6);

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
    assert(is_connected(output_0, spider_0));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_1));

    // test spider 0
    assert(spider_0->edge_count == 2);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == GREEN);
    assert(spider_0->phase == 0.0);
    assert(is_connected(spider_0, input_0));
    assert(is_connected(spider_0, output_0));

    // test spider 1
    assert(spider_1->edge_count == 2);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == (float) M_PI);
    assert(is_connected(spider_1, input_1));
    assert(is_connected(spider_1, output_1));

    free_graph(graph);

    printf("Pass\n");
}

void test_clean_io()
{
    printf("Testing clean_io: ");

    // given
    ZXGraph *graph = initialise_graph(3);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *input_2 = get_node(graph->inputs[2], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *output_2 = get_node(graph->outputs[2], graph);
    Node *spider_0 = initialise_spider(GREEN, 0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    insert_node(spider_0, input_1, output_1);
    insert_node(hadamard_0, input_2, output_2);
    insert_node(spider_1, hadamard_0, output_2);
    insert_node(hadamard_1, spider_1, output_2);

    // when
    clean_io(graph);

    // then
    // direct input to output connections
    Node *spider_2 = graph->nodes[10];
    Node *spider_3 = graph->nodes[11];
    Node *spider_4 = graph->nodes[12];
    Node *hadamard_2 = graph->nodes[13];
    Node *hadamard_3 = graph->nodes[14];

    // input to hadamard box
    Node *spider_5 = graph->nodes[15]; //

    // output to hadamard box
    Node *spider_6 = graph->nodes[16]; //
    
    // input/output connected to same neighbour
    Node *spider_7 = graph->nodes[17]; //
    Node *spider_8 = graph->nodes[18]; //
    Node *spider_9 = graph->nodes[23]; // not a spider
    Node *spider_10 = graph->nodes[24]; // not a spider
    Node *hadamard_4 = graph->nodes[21]; //
    Node *hadamard_5 = graph->nodes[22]; //
    Node *hadamard_6 = graph->nodes[19]; // not a hadamard //
    Node *hadamard_7 = graph->nodes[20]; // not a hadamard //

    // Test graph
    assert(graph->num_qubits == 3);
    assert(graph->num_nodes == 25);

    // test input 0
    assert(input_0->edge_count == 1);
    assert(input_0->type == INPUT);
    assert(is_connected(input_0, spider_2));


    // test input 1
    assert(input_1->edge_count == 1);
    assert(input_1->type == INPUT);
    assert(is_connected(input_1, spider_5));

    // test input 2
    assert(input_2->edge_count == 1);
    assert(input_2->type == INPUT);
    assert(is_connected(input_2, spider_9));

    // test output 0
    assert(output_0->edge_count == 1);
    assert(output_0->type == OUTPUT);
    assert(is_connected(output_0, spider_4));

    // test output 1
    assert(output_1->edge_count == 1);
    assert(output_1->type == OUTPUT);
    assert(is_connected(output_1, spider_7));

    // test output 2
    assert(output_2->edge_count == 1);
    assert(output_2->type == OUTPUT);
    assert(is_connected(output_2, spider_10));

    // test spider 0
    assert(spider_0->edge_count == 2);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == GREEN);
    assert(spider_0->phase == 0.0);
    assert(is_connected(spider_0, hadamard_5));
    assert(is_connected(spider_0, hadamard_7));

    // test spider 1
    assert(spider_1->edge_count == 2);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == 0.0);
    assert(is_connected(spider_1, hadamard_0));
    assert(is_connected(spider_1, hadamard_1));

    // test spider 2
    assert(spider_2->edge_count == 2);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == 0.0);
    assert(is_connected(spider_2, input_0));
    assert(is_connected(spider_2, hadamard_2));

    // test spider 3
    assert(spider_3->edge_count == 2);
    assert(spider_3->type == SPIDER);
    assert(spider_3->color == GREEN);
    assert(spider_3->phase == 0.0);
    assert(is_connected(spider_3, hadamard_2));
    assert(is_connected(spider_3, hadamard_3));

    // test spider 4
    assert(spider_4->edge_count == 2);
    assert(spider_4->type == SPIDER);
    assert(spider_4->color == GREEN);
    assert(spider_4->phase == 0.0);
    assert(is_connected(spider_4, hadamard_3));
    assert(is_connected(spider_4, output_0));

    // test spider 5
    assert(spider_5->edge_count == 2);
    assert(spider_5->type == SPIDER);
    assert(spider_5->color == GREEN);
    assert(spider_5->phase == 0.0);
    assert(is_connected(spider_5, input_1));
    // assert(is_connected(spider_5, hadamard_4));

    // test spider 6
    assert(spider_6->edge_count == 2);
    assert(spider_6->type == SPIDER);
    assert(spider_6->color == GREEN);
    assert(spider_6->phase == 0.0);
    assert(is_connected(spider_6, hadamard_7));
    assert(is_connected(spider_6, hadamard_6));

    // test spider 7
    assert(spider_7->edge_count == 2);
    assert(spider_7->type == SPIDER);
    assert(spider_7->color == GREEN);
    assert(spider_7->phase == 0.0);
    assert(is_connected(spider_7, hadamard_4));
    assert(is_connected(spider_7, output_1));

    // test spider 8
    assert(spider_8->edge_count == 2);
    assert(spider_8->type == SPIDER);
    assert(spider_8->color == GREEN);
    assert(spider_8->phase == 0.0);
    assert(is_connected(spider_8, hadamard_5));
    assert(is_connected(spider_8, hadamard_4));
    

    // test spider 9
    assert(spider_9->edge_count == 2);
    assert(spider_9->type == SPIDER);
    assert(spider_9->color == GREEN);
    assert(spider_9->phase == 0.0);
    assert(is_connected(spider_9, input_2));
    assert(is_connected(spider_9, hadamard_0));

    // test spider 10
    assert(spider_10->edge_count == 2);
    assert(spider_10->type == SPIDER);
    assert(spider_10->color == GREEN);
    assert(spider_10->phase == 0.0);
    assert(is_connected(spider_10, hadamard_1));
    assert(is_connected(spider_10, output_2));

    // test hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, spider_9));
    assert(is_connected(hadamard_0, spider_1));

    // test hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, spider_1));
    assert(is_connected(hadamard_1, spider_10));

    // test hadamard 2
    assert(hadamard_2->edge_count == 2);
    assert(hadamard_2->type == HADAMARD_BOX);
    assert(is_connected(hadamard_2, spider_2));
    assert(is_connected(hadamard_2, spider_3));

    // test hadamard 3
    assert(hadamard_3->edge_count == 2);
    assert(hadamard_3->type == HADAMARD_BOX);
    assert(is_connected(hadamard_3, spider_3));
    assert(is_connected(hadamard_3, spider_4));

    // test hadamard 4
    assert(hadamard_4->edge_count == 2);
    assert(hadamard_4->type == HADAMARD_BOX);
    assert(is_connected(hadamard_4, spider_7));
    assert(is_connected(hadamard_4, spider_8));

    // test hadamard 5
    assert(hadamard_5->edge_count == 2);
    assert(hadamard_5->type == HADAMARD_BOX);
    assert(is_connected(hadamard_5, spider_8));
    assert(is_connected(hadamard_5, spider_0));

    // test hadamard 6
    assert(hadamard_6->edge_count == 2);
    assert(hadamard_6->type == HADAMARD_BOX);
    assert(is_connected(hadamard_6, spider_5));
    assert(is_connected(hadamard_6, spider_6));

    // test hadamard 7
    assert(hadamard_7->edge_count == 2);
    assert(hadamard_7->type == HADAMARD_BOX);
    assert(is_connected(hadamard_7, spider_0));
    assert(is_connected(hadamard_7, spider_6));

    free_graph(graph);

    printf("Pass\n");
}

int main()
{
    test_circuit_to_zx_graph();
    test_remove_z_spiders();
    test_add_hadamard_edges();
    test_clean_edges();
    test_clean_io();
}
