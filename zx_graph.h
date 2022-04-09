#ifndef _ZX_GRAPH_H
#define _ZX_GRAPH_H

typedef enum {HADAMARD, SPIDER, INPUT, OUTPUT} Type;
typedef enum {GREEN, RED} Color;

typedef struct ZXGraph
{
    int qubit_count;
    int node_count;
    struct Node *nodes;
    struct Node **inputs;
    struct Node **outputs;
} ZXGraph;

typedef struct Node
{
    int id;
    int edge_count;
    Type type;
    Color color;
    float phase;
    struct Node **edges;
} Node;

ZXGraph *initialise_graph(int);
void free_graph(ZXGraph *);
Node *initialise_hadamard();
Node *initialise_spider(Color, float);
Node *initialise_input();
Node *initialise_output();
Node *get_node(int, ZXGraph *);
void free_node(Node *);
void insert_node(Node *, Node **, ZXGraph *);
void add_node(Node *, Node *, int, ZXGraph *);
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

#endif
