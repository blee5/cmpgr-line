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
 * So if we want to append a column, we just have add data to the end.
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
            printf("%8.3f ", mt_idx(m, i, j));
        }
        printf(" |\n");
    }
}

/*
 * Matrix multiplication.
 *
 * Given matrices A and B, sets B to AB.
 */
void matrix_mult(struct matrix *a, struct matrix *b)
{
    /* Number of columns in A must match number of columns in B */
    if (a->lastcol != b->rows)
    {
        fprintf(stderr, "Invalid matrix multiplication: %dx%d * %dx%d\n",
                a->rows, a->lastcol, b->rows, b->lastcol);
        exit(1);
    }
    int i, j, k;
    int sum;
    struct matrix *temp = new_matrix(a->rows, b->lastcol);
    temp->lastcol = b->lastcol;
    for (i = 0; i < a->rows; i++)
    {
        for (j = 0; j < b->lastcol; j++)
        {
            sum = 0;
            for (k = 0; k < a->rows; k++)
            {
                sum += mt_idx(a, i, k) * mt_idx(b, k, j);
            }
            mt_idx(temp, i, j) = sum;
        }
    }
    copy_matrix(temp, b);
    free_matrix(temp);
}

/*
 * Turns m into an identity matrix, based on its number of rows.
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
    memset(m->mat, 0, sizeof (double) * m->rows * m->cols);
    for (i = 0; i < m->rows; i++)
    {
        mt_idx(m, i, i) = 1;
    }
    m->lastcol = m->rows;
}

/*
 * Initializes a matrix with 0s.
 */
struct matrix *new_matrix(int rows, int cols)
{
    struct matrix *m = malloc(sizeof (struct matrix));
    if (m == NULL)
    {
        perror("Could not allocate memory for matrix!");
        exit(1);
    }
    m->mat = calloc(sizeof (double), rows * cols);
    if (m->mat == NULL)
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
 * Frees a matrix.
 */
void free_matrix(struct matrix *m)
{
    free(m->mat);
    free(m);
}

/*
 * Adds columns to the matrix, with the amount to add specified by newcols.
 */
void grow_matrix(struct matrix *m, int newcols)
{
    size_t oldlen = m->rows * m->cols;
    m->mat = realloc(m->mat, (oldlen + newcols * m->rows) * sizeof (double));
    /* Initialize new elements to zero */
    memset(m->mat + oldlen, 0, 4 * sizeof (double));
    if (m->mat == NULL)
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
    memcpy(dest->mat, src->mat, src->rows * src->cols * sizeof (double));
    dest->rows = src->rows;
    dest->cols = src->cols;
    dest->lastcol = src->lastcol;
}
