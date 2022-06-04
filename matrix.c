#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"


void print_matrix(struct matrix* mtx_p) {
    for (int i = 0; i < mtx_p->rows; i++) {
        for (int j = 0; j < mtx_p->columns; j++)
        {
            printf("%.10f\t", mtx_p->start[i][j]);
        }
        printf("\n");
    }
}


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

    printf("Insert number of columns M: ");
    ret = fgets(input_buffer, MAX_INPUT_LEN, stdin);
    if(ret == NULL) {
        printf("Error: Invalid input\n");
        return 1;
    }
    mtx_p->M = atoi(input_buffer);

    mtx_p->size = mtx_p->N * mtx_p->M; // Compute total number of elements
    return 0;
}