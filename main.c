#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_LEN 256

struct matrix{
    int N; // rows
    int M; // columns
    int size; // N * M
    double* start;
};

int load_matrix(char* file_name, struct matrix* mtx) {
    FILE* fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int rows_counter = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error: File %s cannot be open\n", file_name);
        return 1;
    }

    mtx->start = (double*)malloc(mtx->size*sizeof(double));
    if (mtx->start == NULL) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }

    // https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    while ((read = getline(&line, &len, fp)) != -1) {
        if (rows_counter > mtx->size) {
            printf("Error: Matrix is larger than specified\n");
            free(mtx->start);
            mtx->start = NULL;
            return 1;
        }

        *(mtx->start + rows_counter) = atof(line);
        rows_counter++;
    }

    if (rows_counter < mtx->size) {
        printf("Error: Matrix is smaller than specified\n");
        free(mtx->start);
        mtx->start = NULL;
        return 1;
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    return 0;
}

void read_matrix_size(struct matrix* mtx) {
    char input_buffer[MAX_INPUT_LEN];

    printf("Insert number of rows N: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    mtx->N = atoi(input_buffer);
    //printf("N: %d\n", mtx->N);

    printf("Insert number of columns M: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    mtx->M = atoi(input_buffer);
    //printf("M: %d\n", mtx->M);

    mtx->size = mtx->N * mtx->M; // Compute total number of elements
}

int main(int argc, char const *argv[])
{
    struct matrix matrix_A, matrix_B, matrix_C;

    // Ask for matrix A size
    printf("Matrix A\n");
    read_matrix_size(&matrix_A);
    
    // Ask for matrix B size
    printf("Matrix B\n");
    read_matrix_size(&matrix_B);

    // Check for valid multiplication
    if (matrix_A.M != matrix_B.N) {
        printf("Error: Matrices cannot be multiplied!\n");
        return 1;
    }

    // Load matrices
    int ret;
    ret = load_matrix("test/matrixA.txt", &matrix_A);
    ret = load_matrix("test/matrixB.txt", &matrix_B);
    if (ret) {
        printf("Error: Matrices were not loaded correctly\n");
    }

    // Multiply matrices
    // Sequential
    matrix_C.N = matrix_A.N;
    matrix_C.M = matrix_B.M;
    matrix_C.size = matrix_C.N * matrix_C.M;
    matrix_C.start = (double*)malloc(matrix_C.size * sizeof(double));
    
    double acum = 0;
    for (int i = 0; i < matrix_A.N; i++) {
        for (int j = 0; j < matrix_B.M; j++) {
            for (int k = 0; k < matrix_B.N; k++) {
                acum += matrix_A.start[k + (i*matrix_A.M)] * matrix_B.start[j + (k*matrix_B.M)];
            }
            matrix_C.start[j + (i*matrix_C.M)] = acum;
            acum = 0;
        }
    }

    // Parallel 1

    // Parallel 2

    // Print result into a file called matrixC.txt
    for (int i = 0; i < matrix_C.N; i++) {
        for (int j = 0; j < matrix_C.M; j++)
        {
            // Replace this with a fprintf to the file
            printf("%.10f\t", matrix_C.start[j + (i*matrix_C.M)]);
        }
        // Format has to 
        printf("\n");
    }

    // Print whether sequential result matches parallel code

    // Print table with time for each execution

    free(matrix_A.start);
    free(matrix_B.start);
    free(matrix_C.start);
    return 0;
}
