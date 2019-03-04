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

void make_translate(struct matrix *transform, double x, double y, double z)
{
    struct matrix *m = ident(4);
    mt_idx(m, 0, 3) = x;
    mt_idx(m, 1, 3) = y;
    mt_idx(m, 2, 3) = z;
    matrix_mult(m, transform);
    free_matrix(m);
}

void make_scale(struct matrix *transform, double x, double y, double z)
{
    struct matrix *m = ident(4);
    mt_idx(m, 0, 0) = x;
    mt_idx(m, 1, 1) = y;
    mt_idx(m, 2, 2) = z;
    matrix_mult(m, transform);
    free_matrix(m);
}

void make_rotX(struct matrix *transform, double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 1, 1) = cost;
    mt_idx(m, 1, 2) = -sint;
    mt_idx(m, 2, 1) = sint;
    mt_idx(m, 2, 2) = cost;
    matrix_mult(m, transform);
    free_matrix(m);
}

void make_rotY(struct matrix *transform, double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 0, 0) = cost;
    mt_idx(m, 0, 2) = sint;
    mt_idx(m, 2, 0) = -sint;
    mt_idx(m, 2, 2) = cost;
    /* print_matrix(m); */
    matrix_mult(m, transform);
    free_matrix(m);
}

void make_rotZ(struct matrix *transform, double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 0, 0) = cost;
    mt_idx(m, 0, 1) = -sint;
    mt_idx(m, 1, 0) = sint;
    mt_idx(m, 1, 1) = cost;
    matrix_mult(m, transform);
    free_matrix(m);
}

void print_matrix(struct matrix *m)
{
    int i, j;
    int rows = m->rows, cols = m->cols;
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
    if (a->cols != b->rows)
    {
        fprintf(stderr, "Invalid matrix multiplication: %dx%d * %dx%d\n",
                a->rows, a->cols, b->rows, b->cols);
        exit(1);
    }
    int i, j, k;
    double sum;
    struct matrix *temp = new_matrix(a->rows, b->cols);
    for (i = 0; i < a->rows; i++)
    {
        for (j = 0; j < b->cols; j++)
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
 * Returns an identity matrix of size n.
 */
struct matrix *ident(int n)
{
    int i;
    struct matrix *m = new_matrix(n, n);
    for (i = 0; i < m->rows; i++)
    {
        mt_idx(m, i, i) = 1;
    }
    return m;
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
}
