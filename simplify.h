#ifndef _SIMPLIFY_H
#define _SIMPLIFY_H

#include "zx_graph.h"
#include "circuit.h"

ZXGraph *circuit_to_zx_graph(Circuit *);
ZXGraph *to_graph_like(ZXGraph *);
ZXGraph *remove_z_spiders(ZXGraph *);
ZXGraph *add_hadamard_edges(ZXGraph *);
ZXGraph *clean_edges(ZXGraph *);
ZXGraph *clean_io(ZXGraph *);
Circuit *zx_graph_to_circuit(ZXGraph *);

#endif
