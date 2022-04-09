#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ZXGraph *initialise_graph(int size)
{
    ZXGraph *graph;
    Node **inputs;
    Node **outputs;
    Node *nodes;
    Node *new_input;
    Node *new_output;

    graph = (ZXGraph *) malloc(sizeof(ZXGraph));
    if(!graph) {
        fprintf(stderr, "error: unable to initialise ZX-Graph.\n");
        exit(EXIT_FAILURE);
    }

    inputs = (Node **) malloc(sizeof(Node *) * size);
    if(!inputs) {
        fprintf(stderr, "error: unable to initialise ZX-Graph inputs.\n");
        exit(EXIT_FAILURE);
    }

    outputs = (Node **) malloc(sizeof(Node *) * size);
    if(!outputs) {
        fprintf(stderr, "error: unable to initialise ZX-Graph outputs.\n");
        exit(EXIT_FAILURE);
    }

    nodes = (Node *) malloc(sizeof(Node)*size*2);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise ZX-Graph nodes.\n");
        exit(EXIT_FAILURE);
    }

    graph -> qubit_count = size;
    graph -> node_count = 0;
    graph -> inputs = inputs;
    graph -> outputs = outputs;
    graph -> nodes = nodes;

    for(int i=0; i<size; i++) {
        new_input = initialise_input(graph -> node_count++);
        new_output = initialise_output(graph -> node_count++);

        new_input -> id = i;
        new_output -> id = size + i;

        new_input -> edges = (Node **) malloc(sizeof(Node *));
        new_input -> edges[0] = new_output;
        new_input -> edge_count = 1;
        
        new_output -> edges = (Node **) malloc(sizeof(Node *));
        new_output -> edges[0] = new_input;
        new_output -> edge_count = 1;

        inputs[i] = new_input;
        outputs[i] = new_output;

        graph -> nodes[i] = *new_input;
        graph -> nodes[size + i] = *new_output;
    }

    return graph;
}

void free_graph(ZXGraph *graph)
{
    if(graph)
        free(graph);
}

Node *initialise_hadamard()
{
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise hadamard node.\n");
        exit(EXIT_FAILURE);
    }

    node -> edge_count = 0;
    node -> type = HADAMARD;

    return node;
}

Node *initialise_spider(Color color, float phase)
{
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise spider node.\n");
        exit(EXIT_FAILURE);
    }

    node -> edge_count = 0;
    node -> type = SPIDER;
    node -> color = color;
    node -> phase = phase;

    return node;
}

Node *initialise_input()
{
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise input node.\n");
        exit(EXIT_FAILURE);
    }

    node -> edge_count = 0;
    node -> type = INPUT;

    return node;
}

Node *initialise_output()
{
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise output node.\n");
        exit(EXIT_FAILURE);
    }

    node -> edge_count = 0;
    node -> type = OUTPUT;

    return node;
}

Node *get_node(int id, ZXGraph *graph)
{
    for(int i=0; i < graph -> node_count; i++)
        if(graph -> nodes[i].id == id)
            return &(graph -> nodes[i]);

    fprintf(stderr, "error: node %d not found.\n", id);
    exit(EXIT_FAILURE);
}

void free_node(Node *node)
{
    if(node)
        free(node);
}

void insert_node(Node *node, Node **edges, ZXGraph *graph)
{
    Node *node_1 = edges[0];
    Node *node_2 = edges[1];
    Node **new_edges;
    Node *new_nodes;

    new_edges = (Node **) malloc(sizeof(Node *) * 2);
    new_edges[0] = node_1;
    new_edges[1] = node_2;

    node -> edges = new_edges;
    node -> edge_count = 2;
    node -> id = graph -> node_count;

    for(int i=0; i < node_1 -> edge_count; i++)
        if(node_1 -> edges[i] == node_2)
            node_1 -> edges[i] = node;

    for(int i=0; i < node_2 -> edge_count; i++)
        if(node_2 -> edges[i] == node_1)
            node_2 -> edges[i] = node;

    graph -> node_count++;
    new_nodes = (Node *) malloc(sizeof(Node) * (graph -> node_count));
    memcpy(new_nodes, graph -> nodes, sizeof(Node) * (graph ->  node_count - 1));
    new_nodes[graph -> node_count - 1] = *node;
    free(graph -> nodes);
    graph -> nodes = new_nodes;

}

// void add_node(Node *node, Node *edges, int num_edges, ZXGraph *graph)
// {
//     Node *new_edges;
//     Node *new_nodes;
//     int new_edge_count;

//     if(node -> edge_count)
//         free(node -> edges);

//     new_edges = (Node *) malloc(sizeof(Node)*num_edges);
    
//     node -> edges = new_edges;
//     node -> edge_count = num_edges;
//     node -> id = graph -> node_count;

//     for(int i=0; i<num_edges; i++)
//         node -> edges[i] = edges[i];

//     for(int i=0; i<num_edges; i++) {
//         new_edge_count = ++edges[i].edge_count;
//         new_edges = (Node *) malloc(sizeof(Node) * new_edge_count);
//         memcpy(new_edges, edges[i].edges, new_edge_count - 1);
//         new_edges[new_edge_count] = *node;
//         free(edges[i].edges);
//         edges[i].edges = new_edges;
//         printf("added edge %d to ");
//     }

//     graph -> node_count++;
//     new_nodes = (Node *) malloc(sizeof(Node) * graph -> node_count);
//     memcpy(new_nodes, graph -> nodes, sizeof(Node) * (graph ->  node_count - 1));
//     free(graph -> nodes);
//     new_nodes[graph -> node_count - 1] = *node;
//     graph -> nodes = new_nodes;
// }
