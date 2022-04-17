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
