#ifndef GMATH_H
#define GMATH_H

#include "image.h"
#include "matrix.h"
#include "parser.h"
#include "symtab.h"

// lighting constants
#define LOCATION 0
#define COLOR 1
#define RED 0
#define GREEN 1
#define BLUE 2

#define SPECULAR_EXP 4

struct lights
{
    struct light light;
    struct lights *next;
};


color get_lighting(double *normal, double *view, color a_light, struct lights *lights, struct constants *refect);
color calculate_ambient(color a_light, struct constants *reflect);
color calculate_diffuse(struct light, struct constants *reflect, double *normal);
color calculate_specular(struct light, struct constants *reflect, double *view, double *normal);

void normalize(double *vector);
double dot_product(double *a, double *b);
double *cross_product(double *a, double *b);
double *calculate_normal(struct matrix *points, int i);

#endif
