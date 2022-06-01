/*
Equipo:
Mario Alberto Ortega Martínez
Dafne Avelin Durón Castán
*/

#include <stdio.h>
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
    for (int i = 0; (i < mtx_p->N + 1) && !EOF_found; i++) {
        for (int j = 0; (j < mtx_p->M + 1) && !EOF_found; j++) {
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

void transpose_matrix(struct matrix* mtx_p){
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
    struct matrix matrix_A, matrix_B, matrix_C;
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


    //printf("MATRIX A:\n");
    //print_matrix(&matrix_A);

    //printf("MATRIX B:\n");
    //print_matrix(&matrix_B);

    // Prepare matrix_C
    matrix_C.N = matrix_A.N;
    matrix_C.M = matrix_B.M;
    matrix_C.size = matrix_C.N * matrix_C.M;
    allocate_matrix(&matrix_C);

    //Transpose matrix_B
    transpose_matrix(&matrix_B);

    // Multiply matrices

    // -.- Sequential -.-
    clock_t start_seq, end_seq;

    start_seq = clock();
    multiply_matrix(&matrix_A, &matrix_B, &matrix_C);
    end_seq = clock();

    printf("Sequential took %ld\n", end_seq - start_seq);

    // -.- Parallel 1 -.-
    clock_t start_autovec, end_autovec;
    start_autovec = clock();
    multiply_matrix_autovec(&matrix_A, &matrix_B, &matrix_C);
    end_autovec = clock();

    printf("Autovectorization took %ld\n", end_autovec - start_autovec);

    // -.- Parallel 2 -.-
    clock_t startOpenMp, endOpenMP;

    int nThreads = 10;
    omp_set_num_threads(nThreads);

    // int i,j,k;
    // double acum = 0;

    startOpenMp = clock();
    multiply_matrix_omp(&matrix_A, &matrix_B, &matrix_C);
    endOpenMP = clock();


    
    printf("OpenMp = %ld\n", endOpenMP - startOpenMp);


    // Print result into a file called matrixC.txt  
    save_matrix(&matrix_C);
    //printf("MATRIX C:\n");
    //print_matrix(&matrix_C);

    // Print whether sequential result matches parallel code

    // Print table with time for each execution

    free_matrix(&matrix_A);
    free_matrix(&matrix_B);
    free_matrix(&matrix_C);
    return 0;
}
