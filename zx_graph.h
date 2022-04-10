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
void apply_antipod();
void apply_fusion();
void apply_color_change();
void apply_id1();
void apply_id2();
void apply_copy();
void apply_bialgebra();
void apply_complementation();
void apply_pivoting();
void apply_boundary();
void free_node(Node *);
void free_graph(ZXGraph *);

#endif
