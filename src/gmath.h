#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"

void normalize(double *vector);
double dot_product(double *a, double *b);
double *cross_product(double *a, double *b);
double *calculate_normal(struct matrix *points, int index);

#endif

