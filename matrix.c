/*
 * Matrix datatype used for storing and manipulating image data.
 *
 * All matrcies will be a 4xN array of doubles.
 *
 * For example, the image will be represented as:
 *
 * x0 x1       xn
 * y0 y1  ...  yn
 * z0 x1       zn
 * 1   1        1
 *
 * Since we will only be appending columns, matrices are stored in a linear array like..
 * [x0, y0, z0, 1, x1, y1, x1, 1, ... , xn, yn, zn, 1]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/*
 * Turns m into an identity matrix.
 * Assumes m is a square matrix.
 */
void ident(struct matrix *m)
{
    int i;
    if (m->rows != m->cols)
    {
        fprintf(stderr, "Attempted to turn a non-square matrix into an identity matrix.\n");
        exit(1);
    }
    /* Clear out matrix */
    memset(m->m, 0, sizeof (double) * m->rows * m->cols);
    for (i = 0; i < m->rows; i++)
    {
        idx_matrix(m, i, i) = 1;
    }
    m->lastcol = m->rows;
}

/*
 * Initializes a matrix
 */
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
        perror("Could not allocate memory for matrix");
        exit(1);
    }
    m->rows = rows;
    m->cols = cols;
    m->lastcol = 0;

    return m;
}

/*
 * Frees a matrix
 */
void free_matrix(struct matrix *m)
{
    free(m->m);
    free(m);
}

/*
 * Adds columns to the matrix, with the number specified by `newcols`.
 */
void grow_matrix(struct matrix *m, int newcols)
{
    size_t oldlen = m->rows * m->cols;
    m->m = realloc(m->m, (oldlen + newcols * m->rows) * sizeof (double));
    /* Initialize new elements to zero */
    memset(m->m + oldlen, 0, 4 * sizeof (double));
    if (m->m == NULL)
    {
        perror("Could not allocate memory for matrix");
        exit(1);
    }
    m->cols += newcols;
}

/*
 * Copies a matrix.
 */
void copy_matrix(struct matrix *src, struct matrix *dest)
{
    memcpy(dest->m, src->m, src->rows * src->cols * sizeof (double));
    dest->rows = src->rows;
    dest->cols = src->cols;
    dest->lastcol = src->lastcol;
}
