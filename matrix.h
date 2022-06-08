#ifndef MATRIX_H
#define MATRIX_H

#define MAX_INPUT_LEN 256
#define OUTPUT_FILE "./matrizC.txt"

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

