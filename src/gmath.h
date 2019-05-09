#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"
#include "image.h"

// lighting constants
#define AMBIENT 0
#define DIFFUSE 1
#define SPECULAR 2
#define LOCATION 0
#define COLOR 1
#define RED 0
#define GREEN 1
#define BLUE 2
#define SPECULAR_EXP 4

color get_lighting(double *normal, double *view, color a_light, double light[2][3],
                   double *a_reflect, double *d_reflect, double *s_reflect);
color calculate_ambient(color a_light, double *a_reflect);
color calculate_diffuse(double light[2][3], double *d_reflect, double *normal);
color calculate_specular(double light[2][3], double *s_reflect, double *view, double *normal);

void normalize(double *vector);
double dot_product(double *a, double *b);
double *cross_product(double *a, double *b);
double *calculate_normal(struct matrix *points, int i);

#endif
