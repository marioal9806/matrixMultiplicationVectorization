/*
-.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.-
-.-             Tecnológico de Monterrey            -.-
-.-                                                 -.-
-.- Materia:                                        -.-
-.- TE3061 Multiprocesadores                        -.-
-.-                                                 -.-
-.- Profesor:                                       -.-
-.- Alejandro Guajardo Moreno                       -.-
-.-                                                 -.-
-.- Equipo:                                         -.-
-.- A01730557 Mario Alberto Ortega Martínez         -.-
-.- A00823833 Dafne Avelin Durón Castán             -.-
-.-                                                 -.-
-.- Fecha: 8.06.2022                                -.-
-.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.-
-.- In matrix.h our subroutines and structures      -.-
-.- used in main.c for matrix multiplication        -.-
-.- are declared.                                   -.-
-.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.- -.-
*/

#ifndef MATRIX_H
#define MATRIX_H

#define MAX_INPUT_LEN 256
#define OUTPUT_FILE "./matrixC.txt"

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

void print_matrix(struct matrix* mtx_p);

void free_matrix(struct matrix* mtx_p);

int allocate_matrix(struct matrix* mtx_p);

int load_matrix(char* file_name, struct matrix* mtx_p);

int save_matrix(struct matrix* mtx_p);

int read_matrix_size(struct matrix* mtx_p);

#endif /* MATRIX_H */

