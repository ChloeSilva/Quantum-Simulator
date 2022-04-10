#ifndef _ZX_GRAPH_H
#define _ZX_GRAPH_H

typedef enum {HADAMARD, SPIDER, INPUT, OUTPUT} Type;
typedef enum {GREEN, RED} Color;

typedef struct ZXGraph
{
    int qubit_count;
    int node_count;
    int *inputs;
    int *outputs;
    struct Node **nodes;
} ZXGraph;

typedef struct Node
{
    int id;
    int edge_count;
    int *edges;
    Type type;
    Color color;
    float phase;
} Node;

ZXGraph *initialise_graph(int);
Node *initialise_input();
Node *initialise_output();
Node *initialise_hadamard(ZXGraph *);
Node *initialise_spider(Color, float, ZXGraph *);
Node *get_node(int, ZXGraph *);
void add_edge(Node *, Node *);
void remove_edge(Node *, Node *);
void insert_node(Node *, Node *, Node *);
void free_node(Node *);
void free_graph(ZXGraph *);

#endif
