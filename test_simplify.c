#include "simplify.h"
#include "circuit.h"
#include "time_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void assert(int status)
{
    if(status == 1)
        return;

    printf("\033[1;31mFailed\n \033[0m");
    exit(EXIT_FAILURE);
}

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
    assert(spider_2->phase == 1.0);
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
    assert(spider_2->phase == 1.0);
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
    assert(spider->phase == 2.0);
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
    assert(spider_1->phase == 1.0);
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

void test_remove_proper_clifford()
{
    printf("Testing remove_proper_clifford: ");

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
    remove_proper_clifford(graph);

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

void test_remove_adjacent_pauli()
{
        printf("Testing remove_adjacent_pauli: ");

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
    remove_adjacent_pauli(graph);

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

void test_remove_boundary_pauli()
{
    printf("Testing remove_boundary_pauli: ");

    // given
    ZXGraph *graph = initialise_graph(1);
    Node *input = get_node(graph->inputs[0], graph);
    Node *output = get_node(graph->outputs[0], graph);
    Node *spider_0 = initialise_spider(GREEN, 0.5, graph);
    Node *spider_1 = initialise_spider(GREEN, 1.0, graph);
    Node *spider_2 = initialise_spider(GREEN, 1.0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    insert_node(spider_0, input, output);
    insert_node(hadamard_0, spider_0, output);
    insert_node(spider_1, hadamard_0, output);
    insert_node(hadamard_1, spider_1, output);
    insert_node(spider_2, hadamard_1, output);

    // when
    remove_boundary_pauli(graph);

    // then
    spider_0 = graph->nodes[5];
    spider_1 = graph->nodes[3];
    spider_2 = graph->nodes[2];
    hadamard_0 = graph->nodes[4];
    hadamard_1 = graph->nodes[6];

    // test graph
    assert(graph->num_nodes == 7);

    // test input
    assert(input->edge_count == 1);
    assert(input->type == INPUT);
    assert(is_connected(input, spider_0));

    // test output
    assert(output->edge_count == 1);
    assert(output->type == OUTPUT);
    assert(is_connected(output, spider_2));

    // test spider 0
    assert(spider_0->edge_count == 2);
    assert(spider_0->type == SPIDER);
    assert(spider_0->color == GREEN);
    assert(spider_0->phase == 0.5);
    assert(is_connected(spider_0, input));
    assert(is_connected(spider_0, hadamard_0));

    // test spider 1
    assert(spider_1->edge_count == 2);
    assert(spider_1->type == SPIDER);
    assert(spider_1->color == GREEN);
    assert(spider_1->phase == 1.0);
    assert(is_connected(spider_1, hadamard_0));
    assert(is_connected(spider_1, hadamard_1));

    // test spider 2
    assert(spider_2->edge_count == 2);
    assert(spider_2->type == SPIDER);
    assert(spider_2->color == GREEN);
    assert(spider_2->phase == 1.0);
    assert(is_connected(spider_2, hadamard_1));
    assert(is_connected(spider_2, output));

    // test hadamard 0
    assert(hadamard_0->edge_count == 2);
    assert(hadamard_0->type == HADAMARD_BOX);
    assert(is_connected(hadamard_0, spider_0));
    assert(is_connected(hadamard_0, spider_1));

    // test hadamard 1
    assert(hadamard_1->edge_count == 2);
    assert(hadamard_1->type == HADAMARD_BOX);
    assert(is_connected(hadamard_1, spider_1));
    assert(is_connected(hadamard_1, spider_2));

    free_graph(graph);

    printf("Pass\n");
}

void test_add_cz_layer()
{
    printf("Testing add_cz_layer: ");

    // given
    Circuit *circuit = initialise_circuit(4);
    ZXGraph *graph = initialise_graph(4);
    Node *input_0 = get_node(graph->inputs[0], graph);
    Node *input_1 = get_node(graph->inputs[1], graph);
    Node *input_2 = get_node(graph->inputs[2], graph);
    Node *input_3 = get_node(graph->inputs[3], graph);
    Node *output_0 = get_node(graph->outputs[0], graph);
    Node *output_1 = get_node(graph->outputs[1], graph);
    Node *output_2 = get_node(graph->outputs[2], graph);
    Node *output_3 = get_node(graph->outputs[3], graph);
    Node *spider_0 = initialise_spider(GREEN, 0.0, graph);
    Node *spider_1 = initialise_spider(GREEN, 0.0, graph);
    Node *spider_2 = initialise_spider(GREEN, 0.0, graph);
    Node *spider_3 = initialise_spider(GREEN, 0.0, graph);
    Node *hadamard_0 = initialise_hadamard(graph);
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    Node *hadamard_3 = initialise_hadamard(graph);

    insert_node(spider_0, input_0, output_0);
    insert_node(spider_1, input_1, output_1);
    insert_node(spider_2, input_2, output_2);
    insert_node(spider_3, input_3, output_3);
    add_edge(hadamard_0, spider_0);
    add_edge(hadamard_0, spider_1);
    add_edge(hadamard_1, spider_0);
    add_edge(hadamard_1, spider_3);
    add_edge(hadamard_2, spider_1);
    add_edge(hadamard_2, spider_3);
    add_edge(hadamard_3, spider_2);
    add_edge(hadamard_3, spider_3);
    
    // when
    add_cz_layer(circuit, graph);

    // then
    TimeStep *step = circuit->steps->first;
    Gate *gate_0 = step->gates[0];
    step = step->next;
    Gate *gate_1 = step->gates[0];
    step = step->next;
    Gate *gate_2 = step->gates[1];
    step = step->next;
    Gate *gate_3 = step->gates[2];

    // test circuit
    assert(circuit->num_qubits == 4);

    // test gate 0
    assert(gate_0->type == Z);
    assert(gate_0->isControlled);
    assert(gate_0->target == 0);
    assert(gate_0->control == 1);

    // test gate 1
    assert(gate_1->type == Z);
    assert(gate_1->isControlled);
    assert(gate_1->target == 0);
    assert(gate_1->control == 3);

    // test gate 2
    assert(gate_2->type == Z);
    assert(gate_2->isControlled);
    assert(gate_2->target == 1);
    assert(gate_2->control == 3);

    // test gate 3
    assert(gate_3->type == Z);
    assert(gate_3->isControlled);
    assert(gate_3->target == 2);
    assert(gate_3->control == 3);

    // test graph
    assert(graph->num_qubits == 4);
    assert(graph->num_nodes == 12);

    // test spider 0
    assert(spider_0->edge_count == 2);
    assert(is_connected(spider_0, input_0));
    assert(is_connected(spider_0, output_0));

    // test spider 1
    assert(spider_1->edge_count == 2);
    assert(is_connected(spider_1, input_1));
    assert(is_connected(spider_1, output_1));

    // test spider 2
    assert(spider_2->edge_count == 2);
    assert(is_connected(spider_2, input_2));
    assert(is_connected(spider_2, output_2));

    // test spider 3
    assert(spider_3->edge_count == 2);
    assert(is_connected(spider_3, input_3));
    assert(is_connected(spider_3, output_3));

    free_graph(graph);
    free_circuit(circuit);

    printf("Pass\n");
}

void test_add_cnot_layer()
{
    printf("Testing add_cnot_layer: ");

    // given
    Circuit *circuit = initialise_circuit(4);
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
    add_cnot_layer(circuit, graph);

    // then
    TimeStep *step = circuit->steps->first;
    Gate *gate_1 = step->gates[0];
    step = step->next;
    Gate *gate_2 = step->gates[1];
    step = step->next;
    Gate *gate_3 = step->gates[3];
    step = step->next;
    Gate *gate_4 = step->gates[2];
    step = step->next;
    Gate *gate_5 = step->gates[1];
    step = step->next;
    Gate *gate_6 = step->gates[2];
    step = step->next;
    Gate *gate_7 = step->gates[3];

    // test circuit
    assert(circuit->num_qubits == 4);

    // test gate 1
    assert(gate_1->type == NOT);
    assert(gate_1->isControlled);
    assert(gate_1->target == 0);
    assert(gate_1->control == 2);

    // test gate 2
    assert(gate_2->type == NOT);
    assert(gate_2->isControlled);
    assert(gate_2->target == 1);
    assert(gate_2->control == 2);
    
    // test gate 3
    assert(gate_3->type == NOT);
    assert(gate_3->isControlled);
    assert(gate_3->target == 3);
    assert(gate_3->control == 2);

    // test gate 4
    assert(gate_4->type == NOT);
    assert(gate_4->isControlled);
    assert(gate_4->target == 2);
    assert(gate_4->control == 1);

    // test gate 5
    assert(gate_5->type == NOT);
    assert(gate_5->isControlled);
    assert(gate_5->target == 1);
    assert(gate_5->control == 2);

    // test gate 6
    assert(gate_6->type == NOT);
    assert(gate_6->isControlled);
    assert(gate_6->target == 2);
    assert(gate_6->control == 0);

    // test gate 7
    assert(gate_7->type == NOT);
    assert(gate_7->isControlled);
    assert(gate_7->target == 3);
    assert(gate_7->control == 2);

    // test graph
    assert(graph->num_qubits == 4);
    assert(graph->num_nodes == 12);

    // test right spider 0
    assert(right_spider_0->edge_count == 2);
    assert(is_connected(right_spider_0, input_0));
    assert(is_connected(right_spider_0, output_0));

    // test right spider 1
    assert(right_spider_1->edge_count == 2);
    assert(is_connected(right_spider_1, input_1));
    assert(is_connected(right_spider_1, output_1));

    // test right spider 2
    assert(right_spider_2->edge_count == 2);
    assert(is_connected(right_spider_2, input_2));
    assert(is_connected(right_spider_2, output_2));

    // test right spider 3
    assert(right_spider_3->edge_count == 2);
    assert(is_connected(right_spider_3, input_3));
    assert(is_connected(right_spider_3, output_3));

    free_circuit(circuit);
    free_graph(graph);

    printf("Pass\n");
}

void test_add_hadamard_layer()
{
    printf("Testing add_hadamard_layer: ");

    // given
    Circuit *circuit = initialise_circuit(3);

    // when
    add_hadamard_layer(circuit);

    // then
    TimeStep *step = circuit->steps->first;
    Gate *gate_0 = step->gates[0];
    Gate *gate_1 = step->gates[1];
    Gate *gate_2 = step->gates[2];

    // test gate 0
    assert(gate_0->type == HADAMARD);
    assert(!gate_0->isControlled);
    assert(gate_0->target == 0);

    // test gate 1
    assert(gate_1->type == HADAMARD);
    assert(!gate_1->isControlled);
    assert(gate_1->target == 1);

    // test gate 2
    assert(gate_2->type == HADAMARD);
    assert(!gate_2->isControlled);
    assert(gate_2->target == 2);

    free_circuit(circuit);

    printf("Pass\n");
}

void test_extract_clifford()
{
    printf("Testing extract_clifford: ");

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
    Node *hadamard_9 = initialise_hadamard(graph);
    Node *hadamard_10 = initialise_hadamard(graph);

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

    add_edge(left_spider_1, left_spider_2);
    insert_node(hadamard_9, left_spider_1, left_spider_2);
    add_edge(right_spider_0, right_spider_3);
    insert_node(hadamard_10, right_spider_0, right_spider_3);

    // when
    Circuit *circuit = extract_clifford(graph);

    // then
    TimeStep *step = circuit->steps->first;
    Gate *gate_0 = step->gates[1];
    step = step->next;
    Gate *gate_1 = step->gates[0];
    step = step->next;
    Gate *gate_2 = step->gates[1];
    step = step->next;
    Gate *gate_3 = step->gates[3];
    step = step->next;
    Gate *gate_4 = step->gates[2];
    step = step->next;
    Gate *gate_5 = step->gates[1];
    step = step->next;
    Gate *gate_6 = step->gates[2];
    step = step->next;
    Gate *gate_7 = step->gates[3];
    Gate *gate_8 = step->gates[0];
    Gate *gate_9 = step->gates[1];
    step = step->next;
    Gate *gate_10 = step->gates[2];
    Gate *gate_11 = step->gates[3];
    step = step->next;
    Gate *gate_12 = step->gates[0];

    // test circuit
    assert(circuit->num_qubits == 4);

    // test gate 0
    assert(gate_0->type == Z);
    assert(gate_0->isControlled);
    assert(gate_0->target == 1);
    assert(gate_0->control == 2);

    // test gate 1
    assert(gate_1->type == NOT);
    assert(gate_1->isControlled);
    assert(gate_1->target == 0);
    assert(gate_1->control == 2);

    // test gate 2
    assert(gate_2->type == NOT);
    assert(gate_2->isControlled);
    assert(gate_2->target == 1);
    assert(gate_2->control == 2);

    // test gate 3
    assert(gate_3->type == NOT);
    assert(gate_3->isControlled);
    assert(gate_3->target == 3);
    assert(gate_3->control == 2);

    // test gate 4
    assert(gate_4->type == NOT);
    assert(gate_4->isControlled);
    assert(gate_4->target == 2);
    assert(gate_4->control == 1);

    // test gate 5
    assert(gate_5->type == NOT);
    assert(gate_5->isControlled);
    assert(gate_5->target == 1);
    assert(gate_5->control == 2);

    // test gate 6
    assert(gate_6->type == NOT);
    assert(gate_6->isControlled);
    assert(gate_6->target == 2);
    assert(gate_6->control == 0);

    // test gate 7
    assert(gate_7->type == NOT);
    assert(gate_7->isControlled);
    assert(gate_7->target == 3);
    assert(gate_7->control == 2);

    // test gate 8
    assert(gate_8->type == HADAMARD);
    assert(!gate_8->isControlled);
    assert(gate_8->target == 0);

    // test gate 9
    assert(gate_9->type == HADAMARD);
    assert(!gate_9->isControlled);
    assert(gate_9->target == 1);

    // test gate 10
    assert(gate_10->type == HADAMARD);
    assert(!gate_10->isControlled);
    assert(gate_10->target == 2);

    // test gate 0
    assert(gate_11->type == HADAMARD);
    assert(!gate_11->isControlled);
    assert(gate_11->target == 3);

    // test gate 12
    assert(gate_12->type == Z);
    assert(gate_12->isControlled);
    assert(gate_12->target == 0);
    assert(gate_12->control == 3);

    free_circuit(circuit);
    free_graph(graph);

    printf("Pass\n");
}

int main()
{
    printf("\033[1;32m");
    
    // circuit to graph
    test_circuit_to_zx_graph();
    
    // circuit to graph-like
    test_remove_z_spiders();
    test_add_hadamard_edges();
    test_clean_edges();
    test_clean_io();

    // simplification
    test_remove_proper_clifford();
    test_remove_adjacent_pauli();
    test_remove_boundary_pauli();

    // circuit extraction
    test_add_cz_layer();
    test_add_cnot_layer();
    test_add_hadamard_layer();
    test_extract_clifford();

    printf("\033[0m");
}
