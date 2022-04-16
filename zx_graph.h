#ifndef _ZX_GRAPH_H
#define _ZX_GRAPH_H

typedef enum {HADAMARD_BOX, SPIDER, INPUT, OUTPUT} Type;
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
void add_phase(Node *, float);
void add_edge(Node *, Node *);
void remove_edge(Node *, Node *);
void remove_node(Node *, ZXGraph *);
void insert_node(Node *, Node *, Node *);
int is_connected(Node *, Node *);
int is_connected_io(Node *, ZXGraph *);
int is_red(Node *);
int is_proper_clifford(Node *);
int is_pauli(Node *);
void remove_hadamard_edges(Node *, Node *, ZXGraph *);
Node **get_hadamard_edge_spiders(Node *, ZXGraph *);

#endif
