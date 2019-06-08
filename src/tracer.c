#include <stdlib.h>

#include "tracer.h"
#include "gmath.h"

void add_object(struct object **objs, struct matrix *polygons, struct constants *constants)
{
    struct object *p;
    if (*objs == NULL)
    {
        p = *objs = malloc(sizeof(struct object));
        p->polygons = new_matrix(4, polygons->cols);
        copy_matrix(polygons, p->polygons);
        p->constants = constants;
        p->next = NULL;
    }
    else
    {
        p = *objs;
        while (p->next) p = p->next;
        p->next = malloc(sizeof(struct object));
        p->next->polygons = new_matrix(4, polygons->cols);
        copy_matrix(polygons, p->next->polygons);
        p->next->constants = constants;
        p->next->next = NULL;
    }
}

#include <stdio.h>

double ray_intersects_polygon(double *ray_orig,
                              double *ray_dir,
                              struct matrix *polygons, int i)
{
    /* Moller-Trumbore algorithm, I have no idea how it works */
    const float EPSILON = 0.00000001;

    /* Calculate normals */
    double v0v1[3], v0v2[3];
    double x0, y0, z0, x1, y1, z1, x2, y2, z2;
    double *p = &mt_idx(polygons, 0, i);
    x0 = *(p++); y0 = *(p++); z0 = *(p++); p++;
    x1 = *(p++); y1 = *(p++); z1 = *(p++); p++;
    x2 = *(p++); y2 = *(p++); z2 = *(p++);

    /* v0 and v1 are the edge vectors */
    v0v1[0] = x1 - x0; v0v1[1] = y1 - y0; v0v1[2] = z1 - z0;
    v0v2[0] = x2 - x0; v0v2[1] = y2 - y0; v0v2[2] = z2 - z0;

    double h[3], q[3];

    cross_product(ray_dir, v0v2, h);
    double det = dot_product(v0v1, h);
    if (-EPSILON < det && det < EPSILON) // ray is parallel
        goto cleanup;

    double s[3];
    s[0] = ray_orig[0] - x0;
    s[1] = ray_orig[1] - y0;
    s[2] = ray_orig[2] - z0;

    double u = dot_product(s, h) / det;
    if (1.0 < u || u < 0.0)
        goto cleanup;

    cross_product(s, v0v1, q);
    double v = dot_product(ray_dir, q) / det;
    if (1.0 < u + v || v < 0.0)
        goto cleanup;

    double t = dot_product(v0v2, q) / det;
    return -t;

cleanup:
    return -1;
}

color trace_ray(double *ray_orig,
                double *ray_dir,
                struct object *objs,
                struct light **lights,
                color a_light)
{
    color background;
    background.r = background.g = background.b = DEFAULT_COLOR;
    double min_dist = 1e8;
    double min_idx = -1;
    struct object *min_object = NULL;

    while (objs)
    {
        struct matrix *polygons = objs->polygons;
        for (int i = 0; i < polygons->lastcol; i += 3)
        {
            double dist = ray_intersects_polygon(ray_orig, ray_dir, polygons, i);
            if (dist < min_dist && dist > 0)
            {
                min_dist = dist;
                min_idx = i;
                min_object = objs;
            }
        }
        objs = objs->next;
    }

    if (min_idx < 0)
        return background;

    struct light *light;
    struct constants *reflect = min_object->constants;
    double normal[3];
    double view[3];
    view[0] = 0;
    view[1] = 0;
    view[2] = 1;
    calculate_normal(min_object->polygons, min_idx, normal);
    return get_lighting(normal, view, a_light, lights, min_object->constants);
}

void render(Image s, struct object *objs, struct light **lights, color a_light)
{
    double ray_orig[3];
    ray_orig[0] = XRES / 2;
    ray_orig[1] = YRES / 2;
    ray_orig[2] = 1000;
    for (int x = 0; x < XRES; ++x)
    {
        for (int y = 0; y < YRES; ++y)
        {
            double prim_ray[3];
            prim_ray[0] = ray_orig[0] - (x + 0.5); // huh??
            prim_ray[1] = (y + 0.5) - ray_orig[1];
            prim_ray[2] = ray_orig[2];
            normalize(prim_ray);
            s[x][y] = trace_ray(ray_orig, prim_ray, objs, lights, a_light);
        }
    }
}

