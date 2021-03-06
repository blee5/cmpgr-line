#include <math.h>
#include <stdlib.h>

#include "gmath.h"
#include "image.h"

color get_lighting(double *normal, double *view, color a_light, double light[2][3], struct constants *reflect)
{
    color a = calculate_ambient(a_light, reflect);
    color d = calculate_diffuse(light, reflect, normal);
    color s = calculate_specular(light, reflect, view, normal);

    color il;
    int r = a.r + d.r + s.r + reflect->red;
    int g = a.g + d.g + s.g + reflect->green;
    int b = a.b + d.b + s.b + reflect->blue;
    il.r = r < MAX_COLOR? r: MAX_COLOR;
    il.g = g < MAX_COLOR? g: MAX_COLOR;
    il.b = b < MAX_COLOR? b: MAX_COLOR;
    return il;
}

color calculate_ambient(color a_light, struct constants *reflect)
{
    a_light.r *= reflect->r[AMBIENT_R];
    a_light.g *= reflect->g[AMBIENT_R];
    a_light.b *= reflect->b[AMBIENT_R];
    return a_light;
}

color calculate_diffuse(double light[2][3], struct constants *reflect, double *normal)
{
    color c;
    double r, g, b;
    c.r = c.g = c.b = 0;
    normalize(normal);
    normalize(light[LOCATION]);
    double diffuse = dot_product(normal, light[LOCATION]);
    if (diffuse < 0)
        return c;
    r = diffuse * reflect->r[DIFFUSE_R] * light[COLOR][RED];
    g = diffuse * reflect->g[DIFFUSE_R] * light[COLOR][GREEN];
    b = diffuse * reflect->b[DIFFUSE_R] * light[COLOR][BLUE];
    c.r = r < MAX_COLOR? r: MAX_COLOR;
    c.g = g < MAX_COLOR? g: MAX_COLOR;
    c.b = b < MAX_COLOR? b: MAX_COLOR;
    return c;
}

color calculate_specular(double light[2][3], struct constants *reflect, double *view, double *normal)
{
    color c;
    c.r = c.g = c.b = 0;
    double r, g, b;
    double reflection[3];
    double diffuse, specular, view_cosine;
    normalize(normal);
    normalize(view);
    normalize(light[LOCATION]);
    diffuse = dot_product(normal, light[LOCATION]);
    for (int i = 0; i < 3; i++)
        reflection[i] = 2 * diffuse * normal[i] - light[LOCATION][i];
    view_cosine = dot_product(reflection, view);
    if (view_cosine < 0)
        return c;
    specular = pow(view_cosine, SHINYNESS);
    r = specular * reflect->r[SPECULAR_R] * light[COLOR][RED];
    g = specular * reflect->g[SPECULAR_R] * light[COLOR][GREEN];
    b = specular * reflect->b[SPECULAR_R] * light[COLOR][BLUE]; 
    c.r = r < MAX_COLOR? r: MAX_COLOR;
    c.g = g < MAX_COLOR? g: MAX_COLOR;
    c.b = b < MAX_COLOR? b: MAX_COLOR;
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
