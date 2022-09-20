#ifndef _SIMPLIFY_H
#define _SIMPLIFY_H

#include "zx_graph.h"
#include "circuit.h"

ZXGraph *circuit_to_zx_graph(Circuit *);
void to_graph_like(ZXGraph *);
void remove_z_spiders(ZXGraph *);
void add_hadamard_edges(ZXGraph *);
void clean_edges(ZXGraph *);
void clean_io(ZXGraph *);
void remove_proper_clifford(ZXGraph *);
void remove_adjacent_pauli(ZXGraph *);
void remove_boundary_pauli(ZXGraph *);
void add_cz_layer(Circuit *, ZXGraph *);
void add_cnot_layer(Circuit *, ZXGraph *);
void add_hadamard_layer(Circuit *);
void simplify_graph_like(ZXGraph *graph);
Circuit *extract_circuit(ZXGraph *);

#endif
