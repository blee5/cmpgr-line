#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

void print_matrix(struct matrix *m)
{
    int i, j;
    int rows = m->rows, cols = m->lastcol;
    for (i = 0; i < rows; i++)
    {
        printf("| ");
        for (j = 0; j < cols; j++)
        {
            printf("%.2f ", idx_matrix(m, i, j));
        }
        printf(" |\n");
    }
}

double idx_matrix(struct matrix *m, int row, int col)
{
    return m->m[row * m->rows + col];
}

void setval_matrix(struct matrix *m, int row, int col, double val)
{
    m->m[row * m->rows + col] = val;
}

/*
 * Turns m into an identity matrix.
 * Assumes m is a square matrix.
 */
void ident(struct matrix *m)
{
    int i;
    for (i = 0; i < m->rows; i++)
    {
        setval_matrix(m, i, i, 1);
    }
}
struct matrix *new_matrix(int rows, int cols)
{
    struct matrix *m = malloc(sizeof (struct matrix));
    if (m == NULL)
    {
        perror("Could not allocate memory for matrix!");
        exit(1);
    }
    m->m = calloc(sizeof (double), rows * cols);
    if (m->m == NULL)
    {
        perror("Could not allocate memory for matrix!");
        exit(1);
    }
    m->rows = rows;
    m->cols = cols;
    m->lastcol = cols;

    return m;
}

void free_matrix(struct matrix *m)
{
    free(m->m);
    free(m);
}
