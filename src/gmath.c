#include <math.h>
#include <stdlib.h>

#include "gmath.h"

/*
 * NOTE: ALL VECTORS ARE ASSUMED TO BE 3D
 */


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

double *calculate_normal(struct matrix *points, int index)
{
    double v0[3], v1[3];
    double x0, y0, z0, x1, y1, z1, x2, y2, z2;
    x0 = mt_idx(points, 0, index);
    y0 = mt_idx(points, 1, index);
    z0 = mt_idx(points, 2, index);

    x1 = mt_idx(points, 0, index + 1);
    y1 = mt_idx(points, 1, index + 1);
    z1 = mt_idx(points, 2, index + 1);

    x2 = mt_idx(points, 0, index + 2);
    y2 = mt_idx(points, 1, index + 2);
    z2 = mt_idx(points, 2, index + 2);

    v0[0] = x1 - x0; v0[1] = y1 - y0; v0[2] = z1 - z0;
    v1[0] = x2 - x0; v1[1] = y2 - y0; v1[2] = z2 - z0;
    return cross_product(v0, v1);
}
