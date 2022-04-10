#include "zx_graph.h"
#include "zx_graph_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_apply_id1()
{
    printf("Testing apply_id1: ");

    ZXGraph *graph;
    Node *input, *output, *spider;

    graph = initialise_graph(1);
    input = get_node(graph->inputs[0], graph);
    output = get_node(graph->outputs[0], graph);
    apply_id1(input, output, RED, graph);

    // test spider
    spider = get_node(2, graph);
    assert(spider->id == 2);
    assert(spider->edge_count == 2);
    assert(spider->edges[0] == input->id);
    assert(spider->edges[1] == output->id);
    assert(spider->type == SPIDER);
    assert(spider->color == RED);
    assert(spider->phase == 0.0);

    // test input's edges
    assert(input->edge_count == 1);
    assert(input->edges[0] == spider->id);
    
    // test output's edges
    assert(output->edge_count == 1);
    assert(output->edges[0] == spider->id);

    printf("Pass\n");
}

int main()
{
    test_apply_id1();
}
