#include "zx_graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Reverses control and target qubits of circuit array.
 * Used to reverse a circuit synthesised from a transposed matrix.
 * 
 * @param circuit The circuit to be flipped
 * @param n The size of the circuit
 */
void flip_control_target(int *circuit, int n)
{
    int temp;

    for(int i=0; i<n; i += 2) {
        temp = circuit[i];
        circuit[i] = circuit[i+1];
        circuit[i+1] = temp;
    }
}

/**
 * @brief Reverses a circuit array.
 * Used to reverse a circuit synthesised from a transposed matrix.
 * 
 * @param circuit The circuit to be flipped
 * @param n The size of the circuit
 */
void reverse(int *circuit, int n)
{
    int temp_1;
    int temp_2;

    for(int i=0; i<n/2; i += 2) {
        temp_1 = circuit[i];
        temp_2 = circuit[i+1];
        circuit[i] = circuit[n-(i+2)];
        circuit[i+1] = circuit[n-(i+1)];
        circuit[n-(i+2)] = temp_1;
        circuit[n-(i+1)] = temp_2;
    }
}

/**
 * @brief Transposes a matrix.
 * Used to synthesise the linear reversible circuit.
 * 
 * @param A The matrix to be transposed
 * @param n The size of the matrix
 */
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

/**
 * @brief Converts a binary array to a decimal integer.
 * Used to identify unique sub-row patterns when applying Gaussian eimination.
 * 
 * @param bin The binary array to be converted
 * @param n The size of the binary array
 * @return The decimal integer corresponding to the binary array
 */
int bin_to_int(int *bin, int n)
{
    int result = 0;

    for(int i=0; i<n; i++) {
        result += pow(2, i) * bin[n-(i+1)];
    }

    return result;
}

/**
 * @brief Synthesised the lower triangle of the biadjacency matrix.
 * Applies modified Gaussian elimination to simplify the lower triangle
 * of the matrix.
 * 
 * @param A The biadjacency matrix of the transformation
 * @param circuit The array to store the circuit
 * @param counter A counter to keep track of the number of gates in the circuit
 * @param n The size of the biadjacency matrix
 */
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

/**
 * @brief Synthesises a linear reverisble circuit given the biadjacency
 * matrix of the circuit's corresponding bipartite graph 
 * 
 * @warning The caller is responsible for freeing the returned circuit
 * @param matrix The biadjacency matrix
 * @param n The size of the biadjacency matrix
 * @param num_cnot An integer pointer in which it stores the number of cnot gates
 * required to synthesise the circuit
 * @return an array which describes a circuit of CNOT gates that corresponds
 *  to a transformation described by a biadjacency matrix
 */
int *synthesise_linear_circuit(int *matrix, int n, int *num_cnot)
{
    int circuit_1[n*n*2];
    int counter_1 = 0;
    int circuit_2[n*n*2];
    int counter_2 = 0;

    // apply synthesis of linear reversible circuits algorithm
    synthesise_lower_triangle(matrix, circuit_1, &counter_1, n);
    transpose(matrix, n);
    synthesise_lower_triangle(matrix, circuit_2, &counter_2, n);

    // flip and reverse second half of circuit as it was transposed
    flip_control_target(circuit_2, counter_2);
    reverse(circuit_2, counter_2);

    // concatenate two halves of circuit
    int *circuit = (int *) malloc(sizeof(int)*(counter_1+counter_2));

    for(int i=0; i<counter_1; i++)
        circuit[i] = circuit_1[i];

    for(int i=0; i<counter_2; i++)
        circuit[counter_1+i] = circuit_2[i];

    reverse(circuit, counter_1+counter_2);
    *num_cnot = counter_1+counter_2;

    return circuit;
}

/**
 * @brief Creates the biadjacency matrix based on a given zx-graph
 * 
 * @warning The caller is responsible for freeing biadjacency matrix
 * @param graph The zx-graph from which we wish to obtain a biadjacency matrix
 * @return a pointer to the biadjacency matrix
 */
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

/**
 * @brief Set the binary representation of a given number
 * as an array of 1s and 0s
 * 
 * @param n The integer to be converted
 * @param size The size of the array
 * @param array The array to store the binary representation
 */
void set_binary(int n, int size, int *array)
{
    for(int i=0; i<size; i++) {
        if(n % (int) pow(2, i+1) == 0) {
            array[i] = 0;
        } else {
            array[i] = 1;
            n -= pow(2, i);
        }
    }
}

/**
 * @brief Returns the sum of an array
 * 
 * @param array The array to be summed
 * @param size The size of the array
 * @return The sum of the array
 */
int get_sum(int *array, int size)
{
    int sum = 0;

    for(int i=0; i<size; i++)
        sum += array[i];

    return sum;
}

/**
 * @brief Returns the index of the first 1 in an array
 * 
 * @param array The array in which to search
 * @return the index of the first 1 in array
 */
int first_one(int *array)
{
    for(int i=0;; i++)
        if(array[i] == 1)
            return i;
}

/**
 * @brief Returns the index of the last 1 in an array
 * 
 * @param array The array in which to search
 * @param n The size of the array
 * @return the index of the last 1 in array
 */
int last_one(int *array, int n)
{
    for(int i=n-1;; i--)
        if(array[i] == 1)
            return i;
}

/**
 * @brief Synthesised a multi-cnot gate.
 * Based on method of xor decomposition
 * 
 * @param n The number of control gates
 * @return a pointer to the synthesised circuit
 */
int *synthesise_multi_cnot(int n)
{
    int *circuit = malloc(sizeof(int)*pow(2,2*n)*n);

    for(int i=0; i<pow(2,2*n)*n; i++)
        circuit[i] = n+1;

    int num_cnot = 0;
    int count = 0;
    int binary[n];

    for(int i=1; i<pow(2, n); i++) {
        
        set_binary(i, n, binary);
        int sum = get_sum(binary, n);
        int parity = (sum-1) % 2;

        while(sum > 0) {
            if(sum == 1) {
                circuit[count++] = first_one(binary);
                binary[first_one(binary)] = 0;
                circuit[count++] = n;
                circuit[count++] = parity;
            } else {
                circuit[count++] = first_one(binary);
                binary[first_one(binary)] = 0;
                circuit[count++] = last_one(binary, n);
                circuit[count++] = parity;
                num_cnot++;
            }
            sum = get_sum(binary, n);
        }

        // undo effect of extra cnot gates
        if(num_cnot != 0) {
            for(int i=0; i<3*num_cnot; i++) {
                circuit[count] = circuit[count-(num_cnot+1)*3];
                count++;
            }
            num_cnot = 0;
        }
    }
    return circuit;
}
