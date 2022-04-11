#ifndef _ZX_GRAPH_RULES_H
#define _ZX_GRAPH_RULES_H

#include "zx_graph.h"

void apply_antipod();
void apply_fusion();
void apply_color_change(Node *, ZXGraph *);
void apply_id1(Node *, Node *, Color, ZXGraph *);
void apply_id2(Node *, Node *, ZXGraph *);
void apply_copy();
void apply_bialgebra();
void apply_complementation();
void apply_pivoting();
void apply_boundary();

#endif
