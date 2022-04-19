#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void reverse(int *circuit, int n)
{
    int temp_1;
    int temp_2;

    for(int i=0; i<n; i += 2) {
        temp_1 = circuit[i];
        circuit[i] = circuit[i+1];
        circuit[i+1] = temp_1;
    }

    for(int i=0; i<n/2; i += 2) {
        temp_1 = circuit[i];
        temp_2 = circuit[i+1];
        circuit[i] = circuit[n-(i+2)];
        circuit[i+1] = circuit[n-(i+1)];
        circuit[n-(i+2)] = temp_1;
        circuit[n-(i+1)] = temp_2;
    }
}

void transpose(int *A, int n)
{
    int T[n][n];

    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            T[i][j] = A[j*n+i];

    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            A [i*n+j]= T[i][j];
}

int bin_to_int(int *bin, int n)
{
    int result = 0;

    for(int i=0; i<n; i++) {
        result += pow(2, i) * bin[n-(i+1)];
    }

    return result;
}

void synthesise_lower_triangle(int *A, int *circuit, int *counter, int n)
{
    int sec_size = 2;

    for (int sec=1; sec<=n/sec_size; sec++)
    {
        // remove duplicate sub-rows in section
        int patt[(int) pow(2,n)];
        for(int i=0; i<pow(2,n); i++)
            patt[i] = -1;

        for(int row_ind=(sec-1)*sec_size; row_ind<n; row_ind++) {
            // sub row pattern defined as interger conversion of binary representation
            //int sub_row_patt = bin_to_int(&A[row_ind][(sec-1)*sec_size], 2);
            int sub_row_patt = bin_to_int(&A[row_ind*n+((sec-1)*sec_size)], sec_size);
            
            if(patt[sub_row_patt] == -1 || sub_row_patt == 0) {
                // new pattern, save index
                patt[sub_row_patt] = row_ind;
            } else {
                // existing pattern, eliminate using previous row
                for(int j=0; j<n; j++)
                    A[row_ind*n+j] ^= A[patt[sub_row_patt]*n+j];
                
                circuit[*counter] = patt[sub_row_patt];
                circuit[*counter+1] = row_ind;
                *counter = *counter + 2;
            }
        }
        
        // Gaussian elimination
        for(int col_ind=(sec-1)*sec_size; col_ind<sec*sec_size; col_ind++)
        {
            // check if diagonal is already set to 1
            int diag_one = 1;
            if(A[col_ind*n+col_ind] == 0)
                diag_one = 0;
            
            // remove 1s in rows below the column index
            for(int row_ind=col_ind+1; row_ind<n; row_ind++) {
                if(A[row_ind*n+col_ind] == 1) {
                    if(diag_one == 0) {
                        
                        for(int i=0; i<n; i++)
                            A[col_ind*n+i] ^= A[row_ind*n+i];
                        
                        circuit[*counter] = row_ind;
                        circuit[*counter+1] = col_ind;
                        *counter = *counter + 2;
                        diag_one = 1;
                    }

                    for(int i=0; i<n; i++)
                        A[row_ind*n+i] ^= A[col_ind*n+i];

                    circuit[*counter] = col_ind;
                    circuit[*counter+1] = row_ind;
                    *counter = *counter + 2;
                }
            }
        }
    }
}

/* returns an array which describes a circuit of CNOT gates that
corresponds to a transformation described by a biadjacency matrix. 
WARNING: caller is responsible for freeing the array. */
int *synthesise_linear_circuit(int *matrix, int n)
{
    int circuit_1[n*n*2];
    int counter_1 = 0;
    int circuit_2[n*n*2];
    int counter_2 = 0;

    synthesise_lower_triangle(matrix, circuit_1, &counter_1, n);
    transpose(matrix, n);
    synthesise_lower_triangle(matrix, circuit_2, &counter_2, n);

    reverse((int *) circuit_2, counter_2);

    int *circuit = (int *) malloc(sizeof(int)*(counter_1+counter_2));

    for(int i=0; i<counter_1; i++)
        circuit[i] = circuit_1[i];

    for(int i=0; i<counter_2; i++)
        circuit[counter_1+i] = circuit_2[i];
    
    return circuit;
}

/* returns the biadjacency matrix of the bipartite graph of a given
circuit in GS-LC form. WARNING: caller is responsible for freeing the
array. */
int *get_biadjacency_matrix(ZXGraph *graph)
{
    // Create an array that return the qubit a node acts on
    int size = graph->num_qubits;
    int qubit[graph->id_counter];

    for(int i=0; i<size; i++) {
        Node *output = get_node(graph->outputs[i], graph);
        qubit[output->edges[0]] = i;
    }

    // Create 0 matrix
    int *matrix = (int *) malloc(sizeof(int)*size*size);

    for(int i=0; i<size*size; i++)
            matrix[i] = 0;

    // Populate biadjacency matrix
    for(int i=0; i<graph->num_qubits; i++) {
        Node *node = get_node(get_node(graph->inputs[i], graph)->edges[0], graph);
        if(node->type != SPIDER) {
            fprintf(stderr, "error: input not connected to spider.\n");
            exit(EXIT_FAILURE);
        }

        Node **neighbours = get_hadamard_edge_spiders(node, graph);
        for(int j=0; j<node->edge_count; j++) {
            if(neighbours[j] == NULL)
                continue;

            matrix[i*size+qubit[neighbours[j]->id]] = 1;
        }

        free(neighbours);
    }

    return matrix;
}
