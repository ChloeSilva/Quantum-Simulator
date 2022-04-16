#ifndef _ZX_GRAPH_RULES_H
#define _ZX_GRAPH_RULES_H

#include "zx_graph.h"

void apply_fusion(Node *, Node *, ZXGraph *);
void apply_color_change(Node *, ZXGraph *);
void apply_id1(Node *, Node *, Color, ZXGraph *);
void apply_id2(Node *, Node *, ZXGraph *);
void apply_local_complement(Node *, ZXGraph *);
void apply_pivot(Node *, Node *, ZXGraph *);
Node *extract_boundary(Node *, ZXGraph *);

#endif
