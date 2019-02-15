#ifndef MATRIX_H
#define MATRIX_H

struct matrix
{
    double *mat;
    int rows, cols;
    int lastcol;
} matrix;

/* Macro to help matrix access */
#define mt_idx(m, r, c) m->mat[(c) * m->rows + (r)]

struct matrix *new_matrix(int rows, int cols);
void free_matrix(struct matrix *m);
void grow_matrix(struct matrix *m, int newcols);
void copy_matrix(struct matrix *src, struct matrix *dest);
void print_matrix(struct matrix *m);
void ident(struct matrix *m);
void matrix_mult(struct matrix *a, struct matrix *b);

#endif
