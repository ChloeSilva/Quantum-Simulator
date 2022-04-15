#include "zx_graph.h"
#include "zx_graph_rules.h"
#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

Node *initialise_node(Gate *gate, ZXGraph *graph)
{
    if(gate->type == HADAMARD)
        return initialise_hadamard(graph);

    if(gate->type == NOT)
        return initialise_spider(RED, (float) M_PI, graph);

    fprintf(stderr, "error: invalid gate type.\n");
    exit(EXIT_FAILURE);
}

ZXGraph *circuit_to_zx_graph(Circuit *circuit)
{
    Node *frontier[circuit->num_qubits];
    Node *output[circuit->num_qubits];
    ZXGraph *graph = initialise_graph(circuit->num_qubits);

    // Set the inputs of the graph to be the frontier of nodes
    for(int i=0; i<circuit->num_qubits; i++) {
        frontier[i] = get_node(graph->inputs[i], graph);
        output[i] = get_node(graph->outputs[i], graph);
    }

    // adds corresponding nodes to graph for each time step
    while(circuit->steps->first) {
        TimeStep *current = circuit->steps->first;    
        for(int i=0; i<circuit->num_qubits; i++) {
            Gate *gate = current->gates[i];
            if(gate->type != NONE && gate->type != CONTROL) {
                if(gate->isControlled) {
                    // add controlled gate
                    Node *target = initialise_spider(RED, 0, graph);
                    Node *control = initialise_spider(GREEN, 0, graph);
                    insert_node(target, frontier[gate->target], output[gate->target]);
                    insert_node(control, frontier[gate->control], output[gate->control]);
                    add_edge(target, control);
                    frontier[gate->target] = target;
                    frontier[gate->control] = control;
                } else {
                    // add regular gate
                    Node *node = initialise_node(gate, graph);
                    insert_node(node, frontier[i], output[i]);
                    frontier[i] = node;
                }
            }
        }
        remove_first_time_step(circuit->steps, circuit->num_qubits);
    }
    
    return graph;
}

ZXGraph *remove_z_spiders(ZXGraph *graph)
{
    for(int i=0; i<graph->num_nodes; i++) {
        Node *current = graph->nodes[i];
        if(is_red(current))
            apply_color_change(current, graph);
    }

    return graph;
}

bool remove_double_hadamard(Node *node, ZXGraph *graph)
{
    // removes double hadamard if present
    // returns true if present and false otherwise
    if(node->type != HADAMARD_BOX)
        return false;

    for(int i=0; i<node->edge_count; i++) {
        Node *neighbour = get_node(node->edges[i], graph);
        if(neighbour->type == HADAMARD_BOX) {
            apply_id2(node, neighbour, graph);
            return true;
        }
    }
    return false;
}

bool fuse_adjacent_spiders(Node *node, ZXGraph *graph)
{
    // fuses spider with adjacent spider if present
    // returns true if present and false otherwises
    if(node->type != SPIDER)
        return false;

    for(int i=0; i<node->edge_count; i++) {
        Node *neighbour = get_node(node->edges[i], graph);
        if(neighbour->type == SPIDER && neighbour->id != node->id) {
            apply_fusion(node, neighbour, graph);
            return true;
        }
    }
    return false;
}

ZXGraph *add_hadamard_edges(ZXGraph *graph)
{
    // remove double hadamard from graph using id2 rule
    bool complete = false;
    while(!complete) {
        complete = true;
        for(int i=0; i<graph->num_nodes; i++)
            if(remove_double_hadamard(graph->nodes[i], graph))
                complete = false;
    }

    // fuse adjacent spiders using fusion rule
    complete = false;
    while(!complete) {
        complete = true;
        for(int i=0; i<graph->num_nodes; i++)
            if(fuse_adjacent_spiders(graph->nodes[i], graph))
                complete = false;
    }

    return graph;
}

bool remove_parallel_edges(Node *node, ZXGraph *graph)
{
    // removes parallel hadamard edges if present
    // return true if present and false otherwise
    if(node->type != SPIDER)
        return false;

    //create array of spiders connected by hadamard edges
    Node *spiders[node->edge_count];

}

bool remove_self_loops(Node *node, ZXGraph *graph)
{
    // removes self loop if present
    // returns true if present and false otherwise
    if(node->type != SPIDER)
        return false;

    for(int i=0; i<node->edge_count; i++) {
        Node *neighbour = get_node(node->edges[i], graph);
        if(neighbour->id == node->id) {
            remove_edge(node, neighbour);
            return true;
        }
    }

    return false;
}

bool remove_hadamard_self_loops(Node *node, ZXGraph *graph)
{
    // removes self loop with hadamard edge if present
    // returns true if present and false otherwise
    if(node->type != SPIDER)
        return false;
    
    for(int i=0; i<node->edge_count; i++) {
        Node *neighbour = get_node(node->edges[i], graph);
        if(neighbour->type == HADAMARD_BOX) {
            if(neighbour->edges[0] == neighbour->edges[1]) {
                remove_node(neighbour, graph);
                add_phase(node, (float) M_PI);
                return true;
            }
        }
    }
    return false;
}

ZXGraph *clean_edges(ZXGraph *graph)
{
    return graph;
}

ZXGraph *clean_io(ZXGraph *graph)
{
    return graph;
}

ZXGraph *to_graph_like(ZXGraph *graph)
{
    graph = remove_z_spiders(graph);
    graph = add_hadamard_edges(graph);
    graph = clean_edges(graph);
    graph = clean_io(graph);
    
    return graph;
}
