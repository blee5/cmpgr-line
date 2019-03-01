#ifndef MATRIX_H
#define MATRIX_H

struct matrix
{
    double *mat;
    int rows, cols;
} matrix;

/* Macro to help matrix access */
#define mt_idx(m, r, c) m->mat[(c) * m->rows + (r)]
#define deg2rad(theta) (theta * 3.141592653 / 180)

/* Transformation matrices */
void clear_transform(struct matrix *transform);
void make_translate(struct matrix *transform, double x, double y, double z);
void make_scale(struct matrix *transform, double x, double y, double z);
void make_rotX(struct matrix *transform, double theta);
void make_rotY(struct matrix *transform, double theta);
void make_rotZ(struct matrix *transform, double theta);

/* Basic matrix manipulation  */
struct matrix *new_matrix(int rows, int cols);
void free_matrix(struct matrix *m);
void grow_matrix(struct matrix *m, int newcols);
void copy_matrix(struct matrix *src, struct matrix *dest);
void print_matrix(struct matrix *m);
struct matrix *ident(int n);
void matrix_mult(struct matrix *a, struct matrix *b);

#endif
