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
#define deg2rad(theta) (theta * M_PI / 180)

/* Transformation matrices */
struct matrix *make_translate(double x, double y, double z);
struct matrix *make_scale(double x, double y, double z);
struct matrix *make_rotX(double theta);
struct matrix *make_rotY(double theta);
struct matrix *make_rotZ(double theta);

/* Basic matrix manipulation  */
struct matrix *new_matrix(int rows, int cols);
void concat_matrix(struct matrix *dest, struct matrix *src);
void free_matrix(struct matrix *m);
void grow_matrix(struct matrix *m, int newcols);
void copy_matrix(struct matrix *src, struct matrix *dest);
void print_matrix(struct matrix *m);
struct matrix *ident(int n);
void matrix_mult(struct matrix *a, struct matrix *b);

#endif
