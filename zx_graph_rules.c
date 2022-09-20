#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Applies the fusion rule of zx-calculus.
 * 
 * @param node_1 first node to be fused
 * @param node_2 second node to be fused
 * @param graph graph nodes belong to
 */
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

    // Add edges between node 1 and node 2's neighbours
    int node_2_edge_count = node_2->edge_count;
    int *node_2_edges = (int *) malloc(sizeof(int)*node_2_edge_count);
    memcpy(node_2_edges, node_2->edges, sizeof(int)*node_2_edge_count);

    for(int i=0; i<node_2_edge_count; i++) {       
        int edge = node_2_edges[i];    
        if(!is_connected(get_node(edge, graph), node_1) && edge != node_1->id) {
            add_edge(node_1, get_node(edge, graph));
        }
    }
    
    free(node_2_edges);

    // Add node 2's phase to node 1 and remove node 2
    node_1->phase += node_2->phase;
    remove_node(node_2, graph);
        
}

/**
 * @brief Applies color change rule of zx-calculus
 * 
 * @param node The node to bee changed
 * @param graph The graph the node belongs to
 */
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
    free(edges);

    change_color(node);
}

/**
 * @brief Applies the id 1 rule of zx-calculus
 * 
 * @param node_1 The node to the left of the new node
 * @param node_2 The node to the right of the new node
 * @param color The color of the new node
 * @param graph The grapht the node belongs to
 */
void apply_id1(Node *node_1, Node *node_2, Color color, ZXGraph *graph)
{
    Node *spider = initialise_spider(color, 0.0, graph);
    
    insert_node(spider, node_1, node_2);
}

/**
 * @brief Applies the id 2 rule of zx-calculus
 * 
 * @param hadamard_1 The first hadamard to remove
 * @param hadamard_2 The second hadamard to remove
 * @param graph The graph to remove them from
 */
void apply_id2(Node *hadamard_1, Node *hadamard_2, ZXGraph *graph)
{
    Node *node_1, *node_2;

    // find first non hadamard connecting node
    if(get_node(hadamard_1->edges[0], graph)->type != HADAMARD_BOX)
        node_1 = get_node(hadamard_1->edges[0], graph);
    else 
        node_1 = get_node(hadamard_1->edges[1], graph);

    // find second non hadamard connecting node
    if(get_node(hadamard_2->edges[0], graph)->type != HADAMARD_BOX)
        node_2 = get_node(hadamard_2->edges[0], graph);
    else 
        node_2 = get_node(hadamard_2->edges[1], graph);

    // apply id2
    remove_node(hadamard_1, graph);
    remove_node(hadamard_2, graph);
    add_edge(node_1, node_2);
}

/**
 * @brief Complements the edges of a node.
 * 
 * @param node The node to be processed
 * @param graph The graph the node belongs to
 */
void complement_edges(Node *node, ZXGraph *graph)
{
    Node **neighbours = get_hadamard_edge_spiders(node, graph);

    for(int i=0; i<node->edge_count; i++) {
        Node *node_1 = neighbours[i];
        if(node_1 == NULL)
            continue;
        
        for(int j=i+1; j<node->edge_count; j++) {
            Node *node_2 = neighbours[j];
            if(node_2 == NULL)
                continue;
            
            Node *hadamard = get_hadamard_edge(node_1, node_2, graph);
            if(hadamard == NULL) {
                hadamard = initialise_hadamard(graph);
                add_edge(hadamard, node_1);
                add_edge(hadamard, node_2);
            } else {
                remove_node(hadamard, graph);
            }
        }
    }

    free(neighbours);
}

/**
 * @brief Applies derived rule 1 of zx-calculus
 * 
 * @param node The node to complement by
 * @param graph The graph to be complemented
 */
void apply_local_complement(Node *node, ZXGraph *graph)
{
    // Update phases
    Node **neighbours = get_hadamard_edge_spiders(node, graph);

    for(int i=0; i<node->edge_count; i++)
        if(neighbours[i])
            add_phase(neighbours[i], -node->phase);
    
    free(neighbours);

    // Update edges
    complement_edges(node, graph);

    // Remove node and its neighbouring hadamards
    int edge_count = node->edge_count;
    int edges[edge_count];
    memcpy(edges, node->edges, sizeof(int)*edge_count);
    
    for(int i=0; i<edge_count; i++)
        remove_node(get_node(edges[i], graph), graph);
    
    remove_node(node, graph);
}

/**
 * @brief Applies derived rule 2 of zx-calculus.
 * 
 * @param node_1 The node to the right of the edge to pivot by
 * @param node_2 The node to the left of the edge to pivot by
 * @param graph The graph to be pivotted
 */
void apply_pivot(Node *node_1, Node *node_2, ZXGraph *graph)
{
    // Update phases
    Node **neighbours_1 = get_hadamard_edge_spiders(node_1, graph);
    Node **neighbours_2 = get_hadamard_edge_spiders(node_2, graph);

    for(int i=0; i<node_1->edge_count; i++) {
        if(neighbours_1[i] && neighbours_1[i] != node_2) {
            add_phase(neighbours_1[i], node_2->phase);
            if(get_hadamard_edge(neighbours_1[i], node_2, graph))
                add_phase(neighbours_1[i], 1.0);
        }
    }

    for(int i=0; i<node_2->edge_count; i++)
        if(neighbours_2[i] && neighbours_2[i] != node_1)
            add_phase(neighbours_2[i], node_1->phase);

    free(neighbours_1);
    free(neighbours_2);

    // Update edges using identity: G ^ uv = G * u * v * u 
    complement_edges(node_1, graph);
    complement_edges(node_2, graph);
    complement_edges(node_1, graph);

    // Remove nodes and their neighbouring hadamards
    int edge_count_1 = node_1->edge_count;
    int edges_1[edge_count_1];
    memcpy(edges_1, node_1->edges, sizeof(int)*edge_count_1);
    
    for(int i=0; i<edge_count_1; i++)
        remove_node(get_node(edges_1[i], graph), graph);

    int edge_count_2 = node_2->edge_count;
    int edges_2[edge_count_2];
    memcpy(edges_2, node_2->edges, sizeof(int)*edge_count_2);

    for(int i=0; i<edge_count_2; i++)
        remove_node(get_node(edges_2[i], graph), graph);
    
    remove_node(node_1, graph);
    remove_node(node_2, graph);
}

/**
 * @brief Extracts the boundary Paulit spider.
 * Used in step 3 of the simplification algorithm.
 * 
 * @param node The node to extract
 * @param graph The graph to exract it from
 * @return pointer to the new phase node
 */
Node *extract_boundary(Node *node, ZXGraph *graph)
{
    Node *io_node;

    for(int i=0; i<node->edge_count; i++) {
        Node *current = get_node(node->edges[i], graph);
        if(current->type == OUTPUT || current->type == INPUT)
            io_node = current;
    }

    Node *hadamard_1 = initialise_hadamard(graph);
    Node *spider_1 = initialise_spider(GREEN, 0.0, graph);
    Node *hadamard_2 = initialise_hadamard(graph);
    Node *spider_2 = initialise_spider(GREEN, node->phase, graph);

    insert_node(hadamard_1, node, io_node);
    insert_node(spider_1, hadamard_1, io_node);
    insert_node(hadamard_2, spider_1, io_node);
    insert_node(spider_2, hadamard_2, io_node);

    change_phase(node, 0.0);

    return node;
}
