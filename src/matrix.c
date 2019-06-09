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

struct matrix *make_translate(double x, double y, double z)
{
    struct matrix *m = ident(4);
    mt_idx(m, 0, 3) = x;
    mt_idx(m, 1, 3) = y;
    mt_idx(m, 2, 3) = z;
    return m;
}

struct matrix *make_scale(double x, double y, double z)
{
    struct matrix *m = ident(4);
    mt_idx(m, 0, 0) = x;
    mt_idx(m, 1, 1) = y;
    mt_idx(m, 2, 2) = z;
    return m;
}

struct matrix *make_rotX(double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 1, 1) = cost;
    mt_idx(m, 1, 2) = -sint;
    mt_idx(m, 2, 1) = sint;
    mt_idx(m, 2, 2) = cost;
    return m;
}

struct matrix *make_rotY(double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 0, 0) = cost;
    mt_idx(m, 0, 2) = sint;
    mt_idx(m, 2, 0) = -sint;
    mt_idx(m, 2, 2) = cost;
    return m;
}

struct matrix *make_rotZ(double theta)
{
    struct matrix *m = ident(4);
    theta = deg2rad(theta);
    double sint = sin(theta), cost = cos(theta);
    mt_idx(m, 0, 0) = cost;
    mt_idx(m, 0, 1) = -sint;
    mt_idx(m, 1, 0) = sint;
    mt_idx(m, 1, 1) = cost;
    return m;
}

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
    int i, j;
    double temp[4];
    for (i = 0; i < b->lastcol; i++)
    {
        memcpy(temp, &mt_idx(b, 0, i), 4 * sizeof(double));
        for (j = 0; j < b->rows; j++)
        {
            mt_idx(b, j, i) = mt_idx(a, j, 0) * temp[0] +
                              mt_idx(a, j, 1) * temp[1] +
                              mt_idx(a, j, 2) * temp[2] +
                              mt_idx(a, j, 3) * temp[3];
        }
    }
}

/*
 * Returns an identity matrix of size n.
 */
struct matrix *ident(int n)
{
    int i;
    struct matrix *m = new_matrix(n, n);
    m->lastcol = n;
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
 * Copy src to dest.
 *
 * dest should have the same number of rows and columns as src.
 */
void copy_matrix(struct matrix *src, struct matrix *dest)
{
    dest->lastcol = src->lastcol;
    memcpy(dest->mat, src->mat, src->rows * src->cols * sizeof (double));
}
