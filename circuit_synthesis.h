#ifndef _CIRCUIT_SYNTHESIS_H
#define _CIRCUIT_SYNTHESIS_H

#include "zx_graph.h"

void reverse(int *, int);
void transpose(int *, int);
int bin_to_int(int *, int);
int *synthesise_linear_circuit(int *, int);
int *get_biadjacency_matrix(ZXGraph *);

#endif
