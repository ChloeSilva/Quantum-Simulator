#include "zx_graph.h"
#include "zx_graph_rules.h"

void apply_id1(Node *node_1, Node *node_2, Color color, ZXGraph *graph)
{
    Node *spider = initialise_spider(color, 0.0, graph);
    insert_node(spider, node_1, node_2);
}
