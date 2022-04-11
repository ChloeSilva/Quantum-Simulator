#ifndef _ZX_GRAPH_H
#define _ZX_GRAPH_H

typedef enum {HADAMARD, SPIDER, INPUT, OUTPUT} Type;
typedef enum {GREEN, RED} Color;

typedef struct ZXGraph
{
    int num_qubits;
    int num_nodes;
    int id_counter;
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
void free_node(Node *);
void free_graph(ZXGraph *);
void change_color(Node *);
void change_phase(Node *, float);
void add_edge(Node *, Node *);
void remove_edge(Node *, Node *);
void remove_node(Node *, ZXGraph *);
void insert_node(Node *, Node *, Node *);
int is_connected(Node *, Node *);

#endif
