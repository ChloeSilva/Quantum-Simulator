#ifndef _SIMPLIFY_H
#define _SIMPLIFY_H

#include "zx_graph.h"
#include "circuit.h"

ZXGraph *circuit_to_zx_graph(Circuit *);
Circuit *zx_graph_to_circuit(ZXGraph *);

#endif
