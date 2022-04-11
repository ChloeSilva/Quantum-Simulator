#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

ZXGraph *initialise_graph(int size)
{
    ZXGraph *graph;
    Node **nodes;
    Node *input_node;
    Node *output_node;
    int *inputs;
    int *outputs;

    // initialise graph
    graph = (ZXGraph *) malloc(sizeof(ZXGraph));
    if(!graph) {
        fprintf(stderr, "error: unable to initialise ZX-Graph.\n");
        exit(EXIT_FAILURE);
    }

    // initialise graph nodes
    nodes = (Node **) malloc(sizeof(Node *)*size*2);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise ZX-Graph nodes.\n");
        exit(EXIT_FAILURE);
    }

    // initialise inputs array
    inputs = (int *) malloc(sizeof(int)*size);
    if(!inputs) {
        fprintf(stderr, "error: unable to initialise inputs array.\n");
        exit(EXIT_FAILURE);
    }

    // initialise outputs array
    outputs = (int *) malloc(sizeof(int)*size);
    if(!outputs) {
        fprintf(stderr, "error: unable to initialise outputs array.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    graph->qubit_count = size;
    graph->node_count = 0;
    graph->inputs = inputs;
    graph->outputs = outputs;
    graph->nodes = nodes;

    // initialise input/output nodes
    for(int i=0; i<size; i++)
    {
        // initialise and add input node
        input_node = initialise_input();
        input_node->id = graph->node_count;
        nodes[graph->node_count] = input_node;
        inputs[i] = input_node->id;
        graph->node_count++;
        
        // initialise and add output node
        output_node = initialise_output();
        output_node->id = graph->node_count;
        nodes[graph->node_count] = output_node;
        outputs[i] = output_node->id;
        graph->node_count++;

        // connect input and output
        add_edge(nodes[graph->node_count-2], nodes[graph->node_count-1]);
    }

    return graph;
}

Node *initialise_input()
{
    Node *node;

    // initialise node
    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise input node.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    node -> edge_count = 0;
    node -> type = INPUT;

    return node;
}

Node *initialise_output()
{
    Node *node;

    // initialise node
    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise output node.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    node -> edge_count = 0;
    node -> type = OUTPUT;

    return node;
}

Node *initialise_hadamard(ZXGraph *graph)
{
    Node *hadamard;
    Node **nodes;

    // Allocate space for new node and add to graph
    hadamard = (Node *) malloc(sizeof(Node));
    if(!hadamard) {
        fprintf(stderr, "error: unable to initialise hadamard node.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate new array of node pointers for graph
    nodes = (Node **) malloc(sizeof(Node *)*graph->node_count+1);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise node pointers.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(nodes, graph->nodes, sizeof(Node *)*graph->node_count);
    free(graph->nodes);
    nodes[graph->node_count] = hadamard;
    graph->nodes = nodes;
    graph->node_count++;

    // set member data
    hadamard->id = graph->node_count-1;
    hadamard->edge_count = 0;
    hadamard->type = HADAMARD;
    
    return hadamard;
}

Node *initialise_spider(Color color, float phase, ZXGraph *graph)
{
    Node *spider;
    Node **nodes;

    // Allocate space for new node and add to graph
    spider = (Node *) malloc(sizeof(Node));
    if(!spider) {
        fprintf(stderr, "error: unable to initialise spider node.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate new array of node pointers for graph
    nodes = (Node **) malloc(sizeof(Node *)*graph->node_count+1);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise new nodes.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(nodes, graph->nodes, sizeof(Node *)*graph->node_count);
    free(graph->nodes);
    nodes[graph->node_count] = spider;
    graph->nodes = nodes;
    graph->node_count++;

    // set member data
    spider->id = graph->node_count-1;
    spider->edge_count = 0;
    spider->type = SPIDER;
    spider->color = color;
    spider->phase = phase;
    
    return spider;
}

Node *get_node(int id, ZXGraph *graph)
{
    for(int i=0; i<graph->node_count; i++)
        if(graph->nodes[i]->id == id)
            return graph->nodes[i];

    fprintf(stderr, "error: node %d not found.\n", id);
    exit(EXIT_FAILURE);
}

void change_color(Node *node)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only change color of spider node.\n");
        exit(EXIT_FAILURE);
    }

    node->color = node->color == RED ? GREEN : RED;
}

void change_phase(Node *node, float phase)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only change color of spider node.\n");
        exit(EXIT_FAILURE);
    }

    node->phase = phase;
}

void add_edge(Node *node_1, Node *node_2)
{
    int *new_edges;

    // Add node_2 to node_1's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_1->edge_count+1));
    new_edges[node_1->edge_count] = node_2->id;
    if(node_1->edge_count) {
        memcpy(new_edges, node_1->edges, sizeof(int)*node_1->edge_count);
        free(node_1->edges);
    }
    node_1->edges = new_edges;
    node_1->edge_count++;

    // Add node_1 to node_2's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_2->edge_count+1));
    new_edges[node_2->edge_count] = node_1->id;
    if(node_2->edge_count) {
        memcpy(new_edges, node_2->edges, sizeof(int)*node_2->edge_count);
        free(node_2->edges);
    }
    node_2->edges = new_edges;
    node_2->edge_count++;
}

void remove_edge(Node *node_1, Node *node_2)
{
    int *new_edges;
    int i, j;
    bool isPresent = false;

    // Check edges are connected
    for(int i=0; i<node_1->edge_count; i++)
        if(node_1->edges[i] == node_2->id)
            isPresent = true;

    if(!isPresent) {
        fprintf(stderr, "error: removing non-existant edge.\n");
        exit(EXIT_FAILURE);
    }

    // Remove node_2 from node_1's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_1->edge_count-1));
    j = 0;
    for(i=0; i<node_1->edge_count; i++) {
        if(node_1->edges[i] != node_2->id) {
            new_edges[j] = node_1->edges[i];
            j++;
        }
    }
    free(node_1->edges);
    node_1->edges = new_edges;
    node_1->edge_count--;

    // Remove node_1 from node_2's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_2->edge_count-1));
    j = 0;
    for(i=0; i<node_2->edge_count; i++) {
        if(node_2->edges[i] != node_1->id) {
            new_edges[j] = node_2->edges[i];
            j++;
        }
    }
    free(node_2->edges);
    node_2->edges = new_edges;
    node_2->edge_count--;
}

void insert_node(Node *node, Node *left_node, Node *right_node)
{
    add_edge(node, left_node);
    add_edge(node, right_node);
    remove_edge(left_node, right_node);
}

void free_node(Node *node)
{
    if(node->edge_count)
        free(node->edges);

    free(node);
}

void free_graph(ZXGraph *graph)
{
    for(int i=0; i<graph->node_count; i++)
        free_node(get_node(i, graph));
    
    free(graph->nodes);
    free(graph->inputs);
    free(graph->outputs);
    free(graph);
}
