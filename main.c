/*
Equipo:
Mario Alberto Ortega Martínez
Dafne Avelin Durón Castán
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_INPUT_LEN 256
#define OUTPUT_FILE "./test/matrixC.txt"

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
    printf("aquí\n");
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
    for (int i = 0; (i < mtx_p->N) && !EOF_found; i++) {
        for (int j = 0; (j < mtx_p->M) && !EOF_found; j++) {
            read = getline(&line, &len, fp);
            if (read == EOF) {
                EOF_found = 1;
            }

            // Check if file has more numbers than expected
            if (rows_counter > mtx_p->size) {
                printf("Error: File has more items than expected\n");
                free_matrix(mtx_p);
                fclose(fp);
                if (line) {
                    free(line);
                }
                return 1;
            }

            mtx_p->start[i][j] = atof(line);
            rows_counter++;
        }
    }
    
    // Check if file has less numbers than expected
    if (rows_counter < mtx_p->size) {
        printf("Error: File has less items than expected\n");
        free_matrix(mtx_p);
        fclose(fp);
        if (line) {
            free(line);
        }
        return 1;
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    return 0;
}

int read_matrix_size(struct matrix* mtx_p) {
    char input_buffer[MAX_INPUT_LEN];
    char* ret = NULL;

    printf("Insert number of rows N: ");
    ret = fgets(input_buffer, MAX_INPUT_LEN, stdin);
    if(ret == NULL) {
        printf("Error: Invalid input\n");
        return 1;
    }
    mtx_p->N = atoi(input_buffer);
    //printf("N: %d\n", mtx->N);

    printf("Insert number of columns M: ");
    ret = fgets(input_buffer, MAX_INPUT_LEN, stdin);
    if(ret == NULL) {
        printf("Error: Invalid input\n");
        return 1;
    }
    mtx_p->M = atoi(input_buffer);
    //printf("M: %d\n", mtx->M);

    mtx_p->size = mtx_p->N * mtx_p->M; // Compute total number of elements
    return 0;
}

/*void transpose_matrix(struct matrix* mtx_p){
    struct matrix m_aux;
    // Aux matrix will have the transposed dimensions of mtx_p
    m_aux.rows = mtx_p->columns;
    m_aux.columns = mtx_p->rows;
    m_aux.size = mtx_p->columns * mtx_p->rows;

    // Allocate space for the auxiliar matrix
    allocate_matrix(&m_aux);

    //Transpose
    for (int i = 0; i < m_aux.columns; ++i){
        for (int j = 0; j < m_aux.rows; ++j) {
            m_aux.start[j][i] = mtx_p->start[i][j];
        }
    }

    int tempC = mtx_p->columns;
    mtx_p->columns = m_aux.columns;
    m_aux.columns = tempC;

    int tempR = mtx_p->rows;
    mtx_p->rows = m_aux.rows;
    m_aux.rows = tempR;

    double* temp = *(mtx_p->start);
    *(mtx_p->start) = *(m_aux.start);
    *(m_aux.start) = temp;

    // Free the space of the auxiliar matrix 
    free_matrix(&m_aux);    
}*/

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

// Disable autovectorization for this function
__attribute__((optimize("no-tree-vectorize")))
void multiply_matrix(struct matrix* mtx_a_p, struct matrix* mtx_b_p, struct matrix* mtx_c_p){
    // Sequential matrix multiplication
    for (int i = 0; i < mtx_a_p->rows; i++) {
        for (int j = 0; j < mtx_b_p->rows; j++) {
            for (int k = 0; k < mtx_b_p->columns; k++) {
                mtx_c_p->start[i][j] += mtx_a_p->start[i][k] * mtx_b_p->start[j][k];
            }
        }
    }
}

void multiply_matrix_autovec(struct matrix* mtx_a_p, struct matrix* mtx_b_p, struct matrix* mtx_c_p) {
    // Optimized matrix multiplication using Autovectorization and AVX2 instruction set
    double acum = 0;
    for (int i = 0; i < mtx_a_p->rows; i++) {
        for (int j = 0; j < mtx_b_p->rows; j++) {
            for (int k = 0; k < mtx_b_p->columns; k++) {
                acum += mtx_a_p->start[i][k] * mtx_b_p->start[j][k];
            }
            mtx_c_p->start[i][j] = acum;
            acum = 0;
        }
    }
}

__attribute__((optimize("no-tree-vectorize")))
void multiply_matrix_omp(struct matrix* mtx_a_p, struct matrix* mtx_b_p, struct matrix* mtx_c_p) {
    double acum = 0;
    #pragma omp parallel
    {
        //int i,j,k;
        #pragma omp for reduction(+:acum)
        for (int i = 0; i < mtx_a_p->rows; i++) {
            for (int j = 0; j < mtx_b_p->rows; j++) {
                for (int k = 0; k < mtx_b_p->columns; k++) {
                    acum += mtx_a_p->start[i][k] * mtx_b_p->start[j][k];
                }
                mtx_c_p->start[i][j] = acum;
                acum = 0;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    struct matrix matrix_A, matrix_B, matrix_C,  matrix_CAutoV,  matrix_COMP;
    int ret = 0;

    // Ask for matrix A size
    printf("\nMatrix A\n");
    ret = read_matrix_size(&matrix_A);
    if (ret) {
        return 1;
    }

    // Ask for matrix B size
    printf("\nMatrix B\n");
    ret = read_matrix_size(&matrix_B);
    if (ret) {
        return 1;
    }

    // Check for valid multiplication
    if (matrix_A.M != matrix_B.N) {
        printf("Error: Matrices cannot be multiplied!\n");
        return 1;
    }

    // Load matrices
    ret |= load_matrix("test/matrixA.txt", &matrix_A);
    ret |= load_matrix("test/matrixB.txt", &matrix_B);
    if (ret) {
        printf("Error: Matrices were not loaded correctly\n");
        return 1;
    }

    // Prepare matrix_C
    matrix_C.N = matrix_CAutoV.N = matrix_COMP.N = matrix_A.N;
    matrix_C.M = matrix_CAutoV.M = matrix_COMP.M = matrix_B.M;
    matrix_C.size = matrix_CAutoV.size = matrix_COMP.size = matrix_C.N * matrix_C.M;
    allocate_matrix(&matrix_C);
    allocate_matrix(&matrix_CAutoV);
    allocate_matrix(&matrix_COMP);

    // -.- -.- -.- -.- Transpose matrix_B -.- -.- -.- -.-
    //transpose_matrix(&matrix_B);

    struct matrix m_aux;

    // Aux matrix will have the transposed dimensions of mtx_p
    m_aux.rows = matrix_B.columns;
    m_aux.columns = matrix_B.rows;
    m_aux.size = matrix_B.columns * matrix_B.rows;

    // Allocate space for the auxiliar matrix
    allocate_matrix(&m_aux);

    //Transpose
    for (int i = 0; i < m_aux.columns; ++i){
        for (int j = 0; j < m_aux.rows; ++j) {
            //memcpy(&m_aux.start[j][i], &matrix_B.start[i][j], sizeof(double));
            m_aux.start[j][i] = matrix_B.start[i][j];
        }
    }

    // -.- -.- -.- -.- Multiply matrices -.- -.- -.- -.- 

    printf("MATRIX A:\n");
    print_matrix(&matrix_A);

    printf("MATRIX B:\n");
    print_matrix(&matrix_B);

    printf("MATRIX AUX:\n");
    print_matrix(&m_aux);

    // -.- Sequential -.-
    clock_t start_seq, end_seq;

    start_seq = clock();
    multiply_matrix(&matrix_A, &m_aux, &matrix_C);
    end_seq = clock();

    printf("Sequential took %ld\n", end_seq - start_seq);
    printf("MATRIX C:\n");
    print_matrix(&matrix_C);
    printf("\n");

    // -.- Parallel 1 -.-
    clock_t start_autovec, end_autovec;
    start_autovec = clock();
    multiply_matrix_autovec(&matrix_A, &m_aux, &matrix_CAutoV);
    end_autovec = clock();

    printf("Autovectorization took %ld\n", end_autovec - start_autovec);
    printf("MATRIX C:\n");
    print_matrix(&matrix_C);
    printf("\n");

    // -.- Parallel 2 -.-
    clock_t startOpenMp, endOpenMP;

    int nThreads = 10;
    omp_set_num_threads(nThreads);

    // int i,j,k;
    // double acum = 0;

    startOpenMp = clock();
    multiply_matrix_omp(&matrix_A, &m_aux, &matrix_COMP);
    endOpenMP = clock();
    
    printf("OpenMp took %ld\n", endOpenMP - startOpenMp);
    printf("MATRIX C:\n");
    print_matrix(&matrix_C);
    printf("\n");


    // Print result into a file called matrixC.txt  
    //save_matrix(&matrix_C);

    // Print whether sequential result matches parallel code
    int flagError = 0;
    for(int i=0; i > matrix_C.rows; i++){
        for(int j=0; j > matrix_C.columns; j++){
            if(matrix_C.start[i][j] != matrix_CAutoV.start[i][j]){
                flagError = 1;
                printf("Error Sequential != Autovec in row: %d, column: %d\n", i , j);
            }else if(matrix_C.start[i][j] != matrix_COMP.start[i][j]){
                flagError = 1;
                printf("Error Sequential != OpenMP in row: %d, column: %d\n", i , j);
            }
        }
    }

    if(flagError == 0){
        printf("FINE :) \n");
    }

    // Print table with time for each execution

    
    free_matrix(&matrix_A);
    printf("A free\n");
    free_matrix(&matrix_B);
    printf("B free\n");
    free_matrix(&matrix_C);
    printf("C free\n");

    free_matrix(&m_aux);
    printf("Aux free\n"); 

    return 0;
}
