#include <stdio.h>
#include <stdlib.h>

// Graph
// [input nodes], [output nodes]

// Node
// Type, Color, Phase, Edges

// Type - emum: hadamard, spider, input, output
// Color - enum: green, red, none
// Phase - float
// Edges - [* node]

typedef enum {hadamard, spider, input, output} TYPE;
typedef enum {green, red} COLOR;

typedef struct Node
{
    float phase;
    TYPE type;
    COLOR color;
    NODE *edges;
} Node;

Circuit *algorithm_4(Graph zx_diagram)
{
    Circuit circuit;

    // convert

    return circuit;
}

void algorithm_3(Graph zx_diagram)
{
    //simplify

    return;
}

void algorithm_2(Graph zx_diagram)
{
    //convert
    
    return;
}

Graph *algorithm_1(Circuit circuit)
{
    Graph *zx_diagram;

    //convert

    return graph;
}

void simplify(Circuit *circuit)
{
    Graph *zx_diagram;

    zx_diagram = algorithm_1(circuit);
    algorithm_2(zx_diagram);
    algorithm_3(zx_diagram);
    circuit = algorithm_4(zx_diagram);

    return;
}

int main()
{
    // Define circuit using circuit data structures
    // Circuit deutsch-jozsa = ...

    // simplify(deutsch-jozsa);

    return 0;
}