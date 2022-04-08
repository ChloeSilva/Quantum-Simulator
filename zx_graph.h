#ifndef _ZX_GRAPH_H
#define _ZX_GRAPH_H

typedef struct ZXGraph
{
    int size;
    struct Node *inputs;
    struct Node *outputs;
} ZXGraph;

typedef struct Node
{
    float phase;
    TYPE type;
    COLOR color;
    struct Node *edges;
} Node;

typedef enum {hadamard, spider, input, output} TYPE;
typedef enum {green, red} COLOR;

#endif