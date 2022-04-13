#include "zx_graph.h"
#include "circuit.h"

#include <stdio.h>
#include <stdlib.h>
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
    while(circuit->steps->head) {
        TimeStep *current = circuit->steps->head;    
        for(int i=0; i<circuit->num_qubits; i++) {
            Gate *gate = current->data[i];
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

        remove_head_linked_list(circuit->steps);
    }
    
    return graph;
}
