#include <math.h>
#include <stdlib.h>

#include "gmath.h"

int clip(int n, int max)
{
    return n < max? n: max;
}

color get_lighting(double *normal, double *view, color a_light, double light[2][3],
                   double *a_reflect, double *d_reflect, double *s_reflect)
{
    color a = calculate_ambient(a_light, a_reflect);
    color d = calculate_diffuse(light, d_reflect, normal);
    color s = calculate_specular(light, s_reflect, view, normal);

    color il;
    il.r = clip(a.r + d.r + s.r, 255);
    il.g = clip(a.g + d.g + s.g, 255);
    il.b = clip(a.b + d.b + s.b, 255);
    return il;
}

color calculate_ambient(color a_light, double *a_reflect)
{
    a_light.r *= a_reflect[RED];
    a_light.g *= a_reflect[GREEN];
    a_light.b *= a_reflect[BLUE];
    return a_light;
}

color calculate_diffuse(double light[2][3], double *d_reflect, double *normal)
{
    color c;
    c.r = c.g = c.b = 0;
    normalize(normal);
    normalize(light[LOCATION]);
    double reflect_val = dot_product(normal, light[LOCATION]);
    if (reflect_val < 0)
        return c;
    double r = reflect_val * d_reflect[RED] * light[COLOR][RED]; 
    double g = reflect_val * d_reflect[GREEN] * light[COLOR][GREEN]; 
    double b = reflect_val * d_reflect[BLUE] * light[COLOR][BLUE]; 
    c.r = clip(r, 255);
    c.g = clip(g, 255);
    c.b = clip(b, 255);
    return c;
}

color calculate_specular(double light[2][3], double *s_reflect, double *view, double *normal)
{
    color c;
    c.r = c.g = c.b = 0;
    double r, g, b;
    normalize(normal);
    normalize(light[LOCATION]);
    normalize(view);

    double reflect_val = dot_product(normal, light[LOCATION]);
    if (reflect_val < 0)
        return c;
    for (int i = 0; i < 3; i++)
        normal[i] = 2 * reflect_val * normal[i] - light[LOCATION][i];
    reflect_val = pow(dot_product(normal, view), 30);
    r = reflect_val * s_reflect[RED] * light[COLOR][RED];
    g = reflect_val * s_reflect[GREEN] * light[COLOR][GREEN];
    b = reflect_val * s_reflect[BLUE] * light[COLOR][BLUE]; 

    c.r = clip(r, 255);
    c.g = clip(g, 255);
    c.b = clip(b, 255);
    return c;
}


/*
 * Normalizes the input vector
 */
void normalize(double *vector)
{
    double mag = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    vector[0] /= mag;
    vector[1] /= mag;
    vector[2] /= mag;
}

/*
 * Returns the dot product of vectors a and b
 */
double dot_product(double *a, double *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/*
 * Returns the cross product of vectors a and b
 */
double *cross_product(double *a, double *b)
{
    double *res = calloc(3, sizeof(double));
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
    return res;
}

/*
 * Calculates the normal for a polygon at the i-th column
 */
double *calculate_normal(struct matrix *points, int i)
{
    double v0[3], v1[3];
    double x0, y0, z0, x1, y1, z1, x2, y2, z2;
    double *p = &mt_idx(points, 0, i);
    x0 = *(p++); y0 = *(p++); z0 = *(p++); p++;
    x1 = *(p++); y1 = *(p++); z1 = *(p++); p++;
    x2 = *(p++); y2 = *(p++); z2 = *(p++);

    v0[0] = x1 - x0; v0[1] = y1 - y0; v0[2] = z1 - z0;
    v1[0] = x2 - x0; v1[1] = y2 - y0; v1[2] = z2 - z0;
    return cross_product(v0, v1);
}
