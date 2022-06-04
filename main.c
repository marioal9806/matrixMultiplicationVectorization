/*
Equipo:
Mario Alberto Ortega Martínez
Dafne Avelin Durón Castán
*/

#include <stdio.h>
#include <time.h>
#include <omp.h>

// Matrix related utilities and definitions
#include "matrix.h"

// Open Source library to print tables to stdout
#include "table.h"


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
    #pragma omp parallel num_threads(10)
    {
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

void print_metrics(clock_t* sequential_metrics, clock_t* autovec_metrics, clock_t* omp_metrics) {
    // https://blog.fourthwoods.com/2013/06/17/pretty-print-tables/
    struct table_t* table = table_init(stdout, (unsigned char*)"Time Metrics Table", 1);

	table_row(table, (unsigned char*)"%s%s%s%s", "Corrida", "Serial", "Paralelo 1 (Autovec)", "Paralelo 2 (OpenMP)");

	table_separator(table);

    table_row(table, (unsigned char*)"%d%u%u%u", 1, sequential_metrics[0], autovec_metrics[0], omp_metrics[0]);
    table_row(table, (unsigned char*)"%d%u%u%u", 2, sequential_metrics[1], autovec_metrics[1], omp_metrics[1]);
    table_row(table, (unsigned char*)"%d%u%u%u", 3, sequential_metrics[2], autovec_metrics[2], omp_metrics[2]);
    table_row(table, (unsigned char*)"%d%u%u%u", 4, sequential_metrics[3], autovec_metrics[3], omp_metrics[3]);
    table_row(table, (unsigned char*)"%d%u%u%u", 5, sequential_metrics[4], autovec_metrics[4], omp_metrics[4]);
    table_separator(table);
    table_row(table, (unsigned char*)"%s%u%u%u", "Promedio", sequential_metrics[5], autovec_metrics[5], omp_metrics[5]);
    table_separator(table);
    table_row(table, (unsigned char*)"%s%s%u%u", "% vs Serial", "-", 
        (unsigned long)(100*((double)sequential_metrics[5]/(double)autovec_metrics[5])), 
        (unsigned long)(100*((double)sequential_metrics[5]/(double)omp_metrics[5])));
	table_commit(table);
}

void measure_method(clock_t* time_array, void (*multiply_method)(struct matrix* mtx_a_p, struct matrix* mtx_b_p, struct matrix* mtx_c_p), 
    struct matrix* mtx_a_p, struct matrix* mtx_b_p, struct matrix* mtx_c_p) {
    clock_t start, end;
    for (int i = 0; i < 5; i++)
    {
        start = clock();
        multiply_method(mtx_a_p, mtx_b_p, mtx_c_p);
        end = clock();
        time_array[i] = end - start;
    }

    clock_t avg = 0;
    for (int i = 0; i < 5; i++)
    {
        avg += time_array[i];
    }

    avg /= 5;
    time_array[5] = avg;
    printf("Average execution time: %ld\n", avg);
}


int main(int argc, char const *argv[])
{
    struct matrix matrix_A, matrix_B, matrix_C, matrix_CAutoV, matrix_COMP;
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
            m_aux.start[j][i] = matrix_B.start[i][j];
        }
    }

    // -.- -.- -.- -.- Multiply matrices -.- -.- -.- -.- 
    // Arrays to store measured execution time. Measure time on 5 runs, save Average on 6th element.
    clock_t time_sequential[6];
    clock_t time_autovec[6];
    clock_t time_omp[6];

    // -.- Sequential -.-
    printf("\n");
    printf("Sequential Multiplication\n");
    measure_method(time_sequential, multiply_matrix, &matrix_A, &m_aux, &matrix_C);
    printf("\n");

    // -.- Parallel 1 -.-
    printf("Autovectorization Multiplication\n");
    measure_method(time_autovec, multiply_matrix_autovec, &matrix_A, &m_aux, &matrix_CAutoV);
    printf("\n");

    // -.- Parallel 2 -.-
    printf("OpenMP Multiplication\n");
    measure_method(time_omp, multiply_matrix_omp, &matrix_A, &m_aux, &matrix_COMP);
    printf("\n");

    // Print result into a file called matrixC.txt  
    save_matrix(&matrix_C);

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
    print_metrics(time_sequential, time_autovec, time_omp);

    free_matrix(&matrix_A);
    free_matrix(&matrix_B);
    free_matrix(&matrix_C);
    free_matrix(&m_aux);

    return 0;
}
