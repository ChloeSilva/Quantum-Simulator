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
    Node *hadamard, *spider, *spider2;
    Node *in_out[2];
    Node *hadamard_spider[2];

    graph = initialise_graph(1);
    hadamard = initialise_hadamard();
    spider = initialise_spider(RED, 1.2);
    spider2 = initialise_spider(GREEN, 1.0);

    in_out[0] = graph -> inputs[0];
    in_out[1] = graph -> outputs[0];
    
    insert_node(hadamard, in_out, graph);
    add_node(spider, in_out, 2, graph);

    hadamard_spider[0] = get_node(2, graph);
    hadamard_spider[1] = get_node(3, graph);

    add_node(spider2, hadamard_spider, 2, graph);

    for(int i=0; i<4; i++) {
        printf("Graph contains node %d with edges ", graph -> nodes[i].id);
        for(int j=0; j < graph -> nodes[i].edge_count; j++) {
            printf("%d ", graph -> nodes[i].edges[j] -> id);
        }
        printf("\n");
    }

    return 0;
}
