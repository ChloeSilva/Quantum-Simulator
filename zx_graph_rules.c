#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void apply_fusion(Node *node_1, Node *node_2, ZXGraph *graph)
{
    if(node_1->type != SPIDER || node_2->type != SPIDER) {
        fprintf(stderr, "error: can only fuse spiders.\n");
        exit(EXIT_FAILURE);
    }

    if(node_1->color != node_2->color) {
        fprintf(stderr, "error: can only fuse spiders of the same color.\n");
        exit(EXIT_FAILURE);
    }

    int node_2_edge_count = node_2->edge_count;
    int *node_2_edges = (int *) malloc(sizeof(int)*node_2_edge_count);
    memcpy(node_2_edges, node_2->edges, sizeof(int)*node_2_edge_count);
}

void apply_color_change(Node *node, ZXGraph *graph)
{
    int edge_count = node->edge_count;
    int *edges = (int *) malloc(sizeof(int)*edge_count);
    memcpy(edges, node->edges, sizeof(int)*edge_count);

    for(int i=0; i<edge_count; i++)
    {
        Node *neighbour = get_node(edges[i], graph);
        Node *hadamard = initialise_hadamard(graph);

        insert_node(hadamard, node, neighbour);
    }

    change_color(node);
}

void apply_id1(Node *node_1, Node *node_2, Color color, ZXGraph *graph)
{
    Node *spider = initialise_spider(color, 0.0, graph);
    
    insert_node(spider, node_1, node_2);
}

void apply_id2(Node *node_1, Node *node_2, ZXGraph *graph)
{
    Node *hadamard_1 = initialise_hadamard(graph);
    Node *hadamard_2 = initialise_hadamard(graph);

    remove_edge(node_1, node_2);
    add_edge(node_1, hadamard_1);
    add_edge(hadamard_1, hadamard_2);
    add_edge(hadamard_2, node_2);
}
