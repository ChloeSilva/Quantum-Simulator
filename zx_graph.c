#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/**
 * @brief Initialises new zx-graph.
 * Allocates memory for new members and sets their default values.
 * 
 * @param size the number of qubits in the graph
 * @return pointer to the graph
 */
ZXGraph *initialise_graph(int size)
{
    ZXGraph *graph;
    Node **nodes;
    Node *input_node;
    Node *output_node;
    int *inputs;
    int *outputs;

    // initialise graph
    graph = (ZXGraph *) malloc(sizeof(ZXGraph));
    if(!graph) {
        fprintf(stderr, "error: unable to initialise ZX-Graph.\n");
        exit(EXIT_FAILURE);
    }

    // initialise graph nodes
    nodes = (Node **) malloc(sizeof(Node *)*size*2);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise ZX-Graph nodes.\n");
        exit(EXIT_FAILURE);
    }

    // initialise inputs array
    inputs = (int *) malloc(sizeof(int)*size);
    if(!inputs) {
        fprintf(stderr, "error: unable to initialise inputs array.\n");
        exit(EXIT_FAILURE);
    }

    // initialise outputs array
    outputs = (int *) malloc(sizeof(int)*size);
    if(!outputs) {
        fprintf(stderr, "error: unable to initialise outputs array.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    graph->num_qubits = size;
    graph->num_nodes = 0;
    graph->id_counter = 0;
    graph->inputs = inputs;
    graph->outputs = outputs;
    graph->nodes = nodes;

    // initialise input/output nodes
    for(int i=0; i<size; i++)
    {
        // initialise and add input node
        input_node = initialise_input();
        input_node->id = graph->id_counter;
        nodes[graph->num_nodes] = input_node;
        inputs[i] = input_node->id;
        graph->num_nodes++;
        graph->id_counter++;
        
        // initialise and add output node
        output_node = initialise_output();
        output_node->id = graph->id_counter;
        nodes[graph->num_nodes] = output_node;
        outputs[i] = output_node->id;
        graph->num_nodes++;
        graph->id_counter++;

        // connect input and output
        add_edge(nodes[graph->num_nodes-2], nodes[graph->num_nodes-1]);
    }

    return graph;
}

/**
 * @brief Initialises input node
 * Allocates memory for new members and sets their default values.
 * 
 * @return pointer to the node
 */
Node *initialise_input()
{
    Node *node;

    // initialise node
    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise input node.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    node -> edge_count = 0;
    node -> type = INPUT;

    return node;
}

Node *initialise_output()
{
    Node *node;

    // initialise node
    node = (Node *) malloc(sizeof(Node));
    if(!node) {
        fprintf(stderr, "error: unable to initialise output node.\n");
        exit(EXIT_FAILURE);
    }

    // set member data
    node -> edge_count = 0;
    node -> type = OUTPUT;

    return node;
}

/**
 * @brief Initialises new hadamard node.
 * Allocates memory for new members and sets their default values.
 * 
 * @param graph 
 * @return Node* 
 */
Node *initialise_hadamard(ZXGraph *graph)
{
    Node *hadamard;
    Node **nodes;

    // Allocate space for new node and add to graph
    hadamard = (Node *) malloc(sizeof(Node));
    if(!hadamard) {
        fprintf(stderr, "error: unable to initialise hadamard node.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate new array of node pointers for graph
    nodes = (Node **) malloc(sizeof(Node *)*graph->num_nodes+1);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise node pointers.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(nodes, graph->nodes, sizeof(Node *)*graph->num_nodes);
    free(graph->nodes);
    nodes[graph->num_nodes] = hadamard;
    graph->nodes = nodes;
    graph->num_nodes++;
    graph->id_counter++;

    // set member data
    hadamard->id = graph->id_counter-1;
    hadamard->edge_count = 0;
    hadamard->type = HADAMARD_BOX;
    
    return hadamard;
}

/**
 * @brief Initialises new spider node
 * Allocates memory for new members and sets their default values.
 * 
 * @param color the color of the spider node
 * @param phase the phase of the spider node
 * @param graph the graph the node belongs to
 * @return pointer to the node
 */
Node *initialise_spider(Color color, float phase, ZXGraph *graph)
{
    Node *spider;
    Node **nodes;

    // Allocate space for new node and add to graph
    spider = (Node *) malloc(sizeof(Node));
    if(!spider) {
        fprintf(stderr, "error: unable to initialise spider node.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate new array of node pointers for graph
    nodes = (Node **) malloc(sizeof(Node *)*graph->num_nodes+1);
    if(!nodes) {
        fprintf(stderr, "error: unable to initialise new nodes.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(nodes, graph->nodes, sizeof(Node *)*graph->num_nodes);
    free(graph->nodes);
    nodes[graph->num_nodes] = spider;
    graph->nodes = nodes;
    graph->num_nodes++;
    graph->id_counter++;

    // set member data
    spider->id = graph->id_counter-1;
    spider->edge_count = 0;
    spider->type = SPIDER;
    spider->color = color;
    spider->phase = phase;
    
    return spider;
}

/**
 * @brief Returns the node of a graph given its id.
 * 
 * @param id The id of the node
 * @param graph the graph the node belongs to
 * @return pointer to the node
 */
Node *get_node(int id, ZXGraph *graph)
{
    for(int i=0; i<graph->num_nodes; i++)
        if(graph->nodes[i]->id == id)
            return graph->nodes[i];

    fprintf(stderr, "error: node %d not found.\n", id);
    exit(EXIT_FAILURE);
}

/**
 * @brief Frees node.
 * 
 * @param node The node to be freed.
 */
void free_node(Node *node)
{
    if(node->edge_count)
        free(node->edges);

    free(node);
}

/**
 * @brief Frees graph and all its member variables.
 * 
 * @param graph The graph to be freed.
 */
void free_graph(ZXGraph *graph)
{
    for(int i=0; i<graph->num_nodes; i++)
        free_node(graph->nodes[i]);
    
    free(graph->nodes);
    free(graph->inputs);
    free(graph->outputs);
    free(graph);
}

/**
 * @brief Changes the color of a node.
 * Checks if the node is a spider and declare error if not. 
 * 
 * @param node the node to be changed
 */
void change_color(Node *node)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only change color of spider node.\n");
        exit(EXIT_FAILURE);
    }

    node->color = node->color == RED ? GREEN : RED;
}

/**
 * @brief Changes the phase of a node.
 * Checks if the node is a spider and declare error if not.
 * 
 * @param node the node to be changed
 * @param phase the new phase of the node
 */
void change_phase(Node *node, float phase)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only change color of spider node.\n");
        exit(EXIT_FAILURE);
    }

    node->phase = phase;
}

/**
 * @brief Adds a float to a node's phase.
 * Checks if the node is a spider and declare error if not.
 * 
 * @param node 
 * @param phase 
 */
void add_phase(Node *node, float phase)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only change color of spider node.\n");
        exit(EXIT_FAILURE);
    }

    node->phase += phase;
}

/**
 * @brief Adds edge beetween 2 nodes.
 * 
 * @param node_1 pointer to the first node
 * @param node_2 pointer to the second node
 */
void add_edge(Node *node_1, Node *node_2)
{
    int *new_edges;

    // Add node_2 to node_1's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_1->edge_count+1));
    new_edges[node_1->edge_count] = node_2->id;
    if(node_1->edge_count) {
        memcpy(new_edges, node_1->edges, sizeof(int)*node_1->edge_count);
        free(node_1->edges);
    }
    node_1->edges = new_edges;
    node_1->edge_count++;

    // Add node_1 to node_2's edge list
    new_edges = (int *) malloc(sizeof(int)*(node_2->edge_count+1));
    new_edges[node_2->edge_count] = node_1->id;
    if(node_2->edge_count) {
        memcpy(new_edges, node_2->edges, sizeof(int)*node_2->edge_count);
        free(node_2->edges);
    }
    node_2->edges = new_edges;
    node_2->edge_count++;
}

/**
 * @brief Removes an edge between 2 nodes.
 * Checks if edge exists and declares error if not.
 * 
 * @param node_1 pointer to the first node
 * @param node_2 pointer to the second node
 */
void remove_edge(Node *node_1, Node *node_2)
{
    int *new_edges;
    bool isPresent = false;

    // Check edges are connected
    for(int i=0; i<node_1->edge_count; i++)
        if(node_1->edges[i] == node_2->id)
            isPresent = true;

    if(!isPresent) {
        fprintf(stderr, "error: removing non-existant edge.\n");
        exit(EXIT_FAILURE);
    }

    // Remove node_2 from node_1's edge list
    bool removed = false;
    if(node_1->edge_count > 1) {
        new_edges = (int *) malloc(sizeof(int)*(node_1->edge_count-1));
        int j = 0;
        for(int i=0; i<node_1->edge_count; i++) {
            if(node_1->edges[i] != node_2->id || removed) {
                new_edges[j] = node_1->edges[i];
                j++;
            } else {
                removed = true;
            }
        }
        free(node_1->edges);
        node_1->edges = new_edges;
    } else {
        free(node_1->edges);
    }
    node_1->edge_count--;

    // Remove node_1 from node_2's edge list
    removed = false;
    if(node_2->edge_count > 1) {
        new_edges = (int *) malloc(sizeof(int)*(node_2->edge_count-1));
        int j = 0;
        for(int i=0; i<node_2->edge_count; i++) {
            if(node_2->edges[i] != node_1->id || removed) {
                new_edges[j] = node_2->edges[i];
                j++;
            } else {
                removed = true;
            }
        }
        free(node_2->edges);
        node_2->edges = new_edges;
    } else {
        free(node_2->edges);
    }
    node_2->edge_count--;
}

/**
 * @brief Removes node from zx-graph and frees graph.
 * 
 * @param node the node to be freed.
 * @param graph the graph from which to free it.
 */
void remove_node(Node *node, ZXGraph *graph)
{
    // Remove edges to all connected nodes
    int edge_count = node->edge_count;
    int *edges = (int *) malloc(sizeof(int)*edge_count);
    memcpy(edges, node->edges, sizeof(int)*edge_count);

    for(int i=0; i<edge_count; i++)
        remove_edge(node, get_node(edges[i], graph));

    free(edges);

    // Update graph
    Node **nodes = (Node **) malloc(sizeof(Node *)*(graph->num_nodes-1));
    int j = 0;
    for(int i=0; i<graph->num_nodes; i++) {
        if(graph->nodes[i] != node) {
            nodes[j] = graph->nodes[i];
            j++;
        }
    }
    free(graph->nodes);
    free_node(node);
    graph->nodes = nodes;
    graph->num_nodes--;
}

/**
 * @brief Inserts node btween two other nodes.
 * Useful for building test graphs
 * 
 * @param node node to be added
 * @param left_node node to the left of the node to be added
 * @param right_node node to the right of the node to be added
 */
void insert_node(Node *node, Node *left_node, Node *right_node)
{
    add_edge(node, left_node);
    add_edge(node, right_node);
    remove_edge(left_node, right_node);
}

/**
 * @brief Checks if two nodes are connected
 * 
 * @param node_1 First node to be checked
 * @param node_2 Second node to be checked
 * @return true if they are connected, false otherwise
 */
int is_connected(Node *node_1, Node *node_2)
{
    for(int i=0; i<node_1->edge_count; i++)
        if(node_1->edges[i] == node_2->id)
            return true;

    return false;
}

/**
 * @brief Checks if a node is connected to an input of output.
 * 
 * @param node The node to be checcked
 * @param graph The graph the node belongs to
 * @return true if it is connected, false otherwise
 */
int is_connected_io(Node *node, ZXGraph *graph)
{
    for(int i=0; i<node->edge_count; i++) {
        Node *neighbour = get_node(node->edges[i], graph);
        if(neighbour->type == INPUT || neighbour->type == OUTPUT)
            return true;
    }

    return false;
}

/**
 * @brief Checks if node is red.
 * 
 * @param node Node to be checked
 * @return true if it is red, false if it's not
 */
int is_red(Node *node)
{
    if(node->type == SPIDER)
        if(node->color == RED)
            return true;

    return false;
}

/**
 * @brief Checks if node is proper clifford.
 * Checks if node is spider and declares error if not.
 * 
 * @param node The Node to be checked
 * @return true if it is proper clifford, false if it's not
 */
int is_proper_clifford(Node *node)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only check phase of spider node.\n");
        exit(EXIT_FAILURE);
    }

    if(floorf(node->phase) == node->phase)
        return false;

    if(floorf(node->phase*2) == node->phase*2)
        return true;

    return false;
}

/**
 * @brief Checks if node is Pauli.
 * Checks if node is spider and declarese error if not.
 * 
 * @param node The Node to be checked
 * @return true if it is pauli, false if it's not
 */
int is_pauli(Node *node)
{
    if(node->type != SPIDER) {
        fprintf(stderr, "error: can only check phase of spider node.\n");
        exit(EXIT_FAILURE);
    }

    if(floorf(node->phase) == node->phase)
        return true;

    return false;
}

/**
 * @brief Removes hadamard edges.
 * 
 * @warning Check if return is NULL beforee using
 * @param node_1 First node
 * @param node_2 Second node
 * @param pointer to the hadamard if the nodes are connected to a hadamard edge,
 * NULL othewise
 */
void remove_hadamard_edges(Node *node_1, Node *node_2, ZXGraph *graph)
{
    int edge_count = node_1->edge_count;
    int edges[edge_count];

    memcpy(edges, node_1->edges, sizeof(int)*edge_count);

    for(int i=0; i<edge_count; i++) {
        Node *current = get_node(edges[i], graph);
        if(current->type == HADAMARD_BOX && is_connected(current, node_2))
            remove_node(current, graph);
    }
}

/**
 * @brief Gets the hadamard between two spiders.
 * 
 * @warning Check if return is NULL beforee using
 * @param node_1 First node
 * @param node_2 Second node
 * @param pointer to the hadamard if the nodes are connected to a hadamard edge,
 * NULL othewise
 */
Node *get_hadamard_edge(Node *node_1, Node *node_2, ZXGraph *graph)
{
    for(int i=0; i<node_1->edge_count; i++) {
        Node *current = get_node(node_1->edges[i], graph);
        if(current->type == HADAMARD_BOX) {
            if(current->edges[0] == node_2->id)
                return current;
            if(current->edges[1] == node_2->id)
                return current;
        }
    }

    return NULL;
}

/**
 * @brief Gets an array of hadamards connected to a spider.
 * 
 * @param node the node
 * @param pointer to the graph the node belongs to
 */
Node **get_hadamard_edge_spiders(Node *node, ZXGraph *graph)
{
    Node **spiders = (Node **) calloc(node->edge_count, sizeof(Node *));

    for(int i=0; i<node->edge_count; i++) {
        Node *current = get_node(node->edges[i], graph);
        if(current->type == HADAMARD_BOX) {
            Node *neighbour;
            if(current->edges[0] != node->id)
                neighbour = get_node(current->edges[0], graph);
            else
                neighbour = get_node(current->edges[1], graph);
            
            if(neighbour->type == SPIDER)
                spiders[i] = neighbour;
        }
    }

    return spiders;
}
