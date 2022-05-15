/*
Equipo:
Mario Alberto Ortega Martínez
Dafne Avelin Durón Castán
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_LEN 256
#define OUTPUT_FILE "matrixC.txt"

struct matrix{
    union {
        int N; // rows
        int rows;
    };
    union {
        int M; // columns
        int columns;
    };
    int size; // N * M
    double** start;
};

void free_matrix(struct matrix* mtx_p) {
    // First, free each row
    for (int i = 0; i < mtx_p->N; i++)
    {
        free(mtx_p->start[i]);
        mtx_p->start[i] = NULL;
    }
    // Then free the double pointer
    free(mtx_p->start);
    mtx_p->start = NULL;
}

int allocate_matrix(struct matrix* mtx_p) {
    // Allocate the necessary rows as pointers to double
    mtx_p->start = (double**)malloc(mtx_p->N*sizeof(double*));
    if (mtx_p->start == NULL) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }

    // Allocate space for every column in each row
    for (int i = 0; i < mtx_p->N; i++)
    {
        mtx_p->start[i] = (double*)malloc(mtx_p->M*sizeof(double));
        if (mtx_p->start[i] == NULL) {
            printf("Error: Memory allocation failed\n");
            return 1;
        }
    }
    return 0;
}

int load_matrix(char* file_name, struct matrix* mtx_p) {
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

    // Allocate space for the matrix
    int ret = 0;
    ret = allocate_matrix(mtx_p);
    if (ret) {
        return 1; // malloc failed
    }
    
    // https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    int EOF_found = 0;
    for (int i = 0; i < mtx_p->N && !EOF_found; i++) {
        for (int j = 0; j < mtx_p->M && !EOF_found; j++) {
            read = getline(&line, &len, fp);
            if (read == EOF) {
                EOF_found = 1;
            }

            // Check if file has more numbers than expected
            if (rows_counter > mtx_p->size) {
                printf("Error: Matrix is larger than specified\n");
                free_matrix(mtx_p);
                return 1;
            }

            mtx_p->start[i][j] = atof(line);
            rows_counter++;
        }
    }
    
    // Check if file has less numbers than expected
    if (rows_counter < mtx_p->size) {
        printf("Error: Matrix is smaller than specified\n");
        free_matrix(mtx_p);
        return 1;
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    return 0;
}

void read_matrix_size(struct matrix* mtx_p) {
    char input_buffer[MAX_INPUT_LEN];

    printf("Insert number of rows N: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    mtx_p->N = atoi(input_buffer);
    //printf("N: %d\n", mtx->N);

    printf("Insert number of columns M: ");
    fgets(input_buffer, MAX_INPUT_LEN, stdin);
    mtx_p->M = atoi(input_buffer);
    //printf("M: %d\n", mtx->M);

    mtx_p->size = mtx_p->N * mtx_p->M; // Compute total number of elements
}

void transpose_matrix(struct matrix* mtx){
    struct matrix m_aux;
    m_aux.rows = mtx ->columns;
    m_aux.columns = mtx ->rows;
    m_aux.size = mtx->columns * mtx->rows;

    // Allocate space for the auxiliar matrix
    allocate_matrix(&m_aux);

    //Transpose
    for (int i = 0; i < m_aux.columns; ++i){
        for (int j = 0; j < m_aux.rows; ++j) {
            m_aux.start[j][i] = mtx->start[i][j];
        }
    }

    int tempC = mtx->columns;
    mtx->columns = m_aux.columns;
    m_aux.columns = tempC;

    int tempR = mtx->rows;
    mtx->rows = m_aux.rows;
    m_aux.rows = tempR;

    double** temp = mtx->start;
    mtx->start = m_aux.start;
    m_aux.start = temp;

    // Free the space of the auxiliar matrix 
    free_matrix(&m_aux);
}

int save_matrix(struct matrix* mtx_p) {
    FILE* fp;

    fp = fopen(OUTPUT_FILE, "w");
    if (fp == NULL) {
        printf("Error: File %s cannot be open\n", OUTPUT_FILE);
        return 1;
    }

    // Print result into a file called matrixC.txt
    for (int i = 0; i < mtx_p->rows; i++) {
        for (int j = 0; j < mtx_p->columns; j++)
        {
            fprintf(fp, "%.10f\n", mtx_p->start[i][j]);
        }
    }

    fclose(fp);
    return 0;
}

void print_matrix(struct matrix* mtx_p) {
    for (int i = 0; i < mtx_p->rows; i++) {
        for (int j = 0; j < mtx_p->columns; j++)
        {
            printf("%.10f\t", mtx_p->start[i][j]);
        }
        printf("\n");
    }
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
    int ret = 0;
    ret |= load_matrix("test/matrixA.txt", &matrix_A);
    ret |= load_matrix("test/matrixB.txt", &matrix_B);
    if (ret) {
        printf("Error: Matrices were not loaded correctly\n");
        return 1;
    }


    printf("MATRIX A:\n");
    print_matrix(&matrix_A);

    printf("MATRIX B:\n");
    print_matrix(&matrix_B);

    // Prepare matrix_C

    //Transpose matrix_B
    transpose_matrix(&matrix_B);

    // Multiply matrices
    // Sequential

    matrix_C.N = matrix_A.N;
    matrix_C.M = matrix_B.M;
    matrix_C.size = matrix_C.N * matrix_C.M;
    allocate_matrix(&matrix_C);

    //Transpose matrix_B
    transpose_matrix(&matrix_B);

    // Multiply matrices

    // -.- Sequential -.-
    
    double acum = 0;
    for (int i = 0; i < matrix_A.rows; i++) {
        for (int j = 0; j < matrix_B.rows; j++) {
            for (int k = 0; k < matrix_B.columns; k++) {
                acum += matrix_A.start[i][k] * matrix_B.start[j][k];
            }
            matrix_C.start[i][j] = acum;
            acum = 0;
        }
    }

    // -.- Parallel 1 -.-

    // -.- Parallel 2 -.-

    // Print result into a file called matrixC.txt  
    save_matrix(&matrix_C);
    printf("MATRIX C:\n");
    print_matrix(&matrix_C);

    // Print whether sequential result matches parallel code

    // Print table with time for each execution

    free_matrix(&matrix_A);
    free_matrix(&matrix_B);
    free_matrix(&matrix_C);
    return 0;
}
