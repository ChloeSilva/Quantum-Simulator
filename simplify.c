#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>

// Circuit *algorithm_4(Graph zx_diagram)
// {
//     Circuit circuit;

//     // convert

//     return circuit;
// }

// void algorithm_3(Graph zx_diagram)
// {
//     //simplify

//     return;
// }

// void algorithm_2(Graph zx_diagram)
// {
//     //convert
    
//     return;
// }

// Graph *algorithm_1(Circuit circuit)
// {
//     Graph *zx_diagram;

//     //convert

//     return graph;
// }

// void simplify(Circuit *circuit)
// {
//     // Graph *zx_diagram;

//     // zx_diagram = algorithm_1(circuit);
//     // algorithm_2(zx_diagram);
//     // algorithm_3(zx_diagram);
//     // circuit = algorithm_4(zx_diagram);

//     return;
// }

int main()
{
    ZXGraph *graph;
    Node *node;
    Node *in_out[2];

    graph = initialise_graph(1);
    node = initialise_hadamard();

    in_out[0] = graph -> inputs[0];
    in_out[1] = graph -> outputs[0];

    for(int i=0; i<2; i++) {
        printf("Graph contains node %d with edges ", (graph -> nodes)[i].id);
        for(int j=0; j < graph -> nodes[i].edge_count; j++) {
            printf("%d ", graph -> nodes[i].edges[j] -> id);
        }
        printf("\n");
    }
    
    insert_node(node, in_out, graph);

    for(int i=0; i<3; i++) {
        printf("Graph contains node %d with edges ", (graph -> nodes)[i].id);
        for(int j=0; j < graph -> nodes[i].edge_count; j++) {
            printf("%d ", graph -> nodes[i].edges[j] -> id);
        }
        printf("\n");
    }

    return 0;
}
