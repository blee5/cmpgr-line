#ifndef MATRIX_H
#define MATRIX_H

struct matrix
{
    double *m;
    int rows, cols;
    int lastcol;
} matrix;


struct matrix *new_matrix(int rows, int cols);
double idx_matrix(struct matrix *m, int row, int col);
void setval_matrix(struct matrix *m, int row, int col, double val);
void free_matrix(struct matrix *m);
void grow_matrix(struct matrix *m, int newcols);
void copy_matrix(struct matrix *src, struct matrix *dest);
void print_matrix(struct matrix *m);
void ident(struct matrix *m);
void matrix_mult(struct matrix *a, struct matrix *b);

#endif
