/*
 * Writing this file made me realize that I probably should have made a structure that represents a vector.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "tracer.h"
#include "gmath.h"

/* used for floating point tests */
const float EPSILON = 0.0001;

void add_object_sphere(struct object **objs, double x, double y, double z, double r, struct constants *constants)
{
    struct object *p = *objs;
    if (p == NULL)
    {
        p = *objs = malloc(sizeof(struct object));
    }
    else
    {
        p = *objs;
        while (p->next) p = p->next;
        p = p->next = malloc(sizeof(struct object));
    }
    p->x = x;
    p->y = y;
    p->z = z;
    p->r = r;
    p->constants = constants;
    p->type = SPHERE_OBJ;
    p->next = NULL;
}

void add_object(struct object **objs, struct matrix *polygons, struct constants *constants)
{
    struct object *p = *objs;
    if (p == NULL)
    {
        p = *objs = malloc(sizeof(struct object));
    }
    else
    {
        p = *objs;
        while (p->next) p = p->next;
        p = p->next = malloc(sizeof(struct object));
    }
    p->polygons = new_matrix(4, polygons->cols);
    copy_matrix(polygons, p->polygons);
    p->constants = constants;
    p->type = MESH_OBJ;
    p->next = NULL;
}

/*
 * Finds the intersection between a ray and a sphere.
 *
 * Returns the smallest distance, or -1 if there is no intersection.
 */
double ray_intersects_sphere(double *ray_orig,
                             double *ray_dir,
                             double x, double y, double z, double r)
{
    double OC[3];
    OC[0] = ray_orig[0] - x;
    OC[1] = ray_orig[1] - y;
    OC[2] = ray_orig[2] - z;

    /* solve the quadratic: */
    /* t^2 * <D, D> + 2<OC, D> t + <OC, OC> - r^2 = 0 */
    double k1, k2, k3, t1, t2, t;
    /* k1 is the dot product of the direction vector by itself, */
    /* but a dot product of a vector by itself is the squre of its magnitude */
    /* and ray_dir has been normalized in render, so just use 1 */
    k1 = 1;
    k2 = 2 * dot_product(OC, ray_dir);
    k3 = dot_product(OC, OC) - r * r;

    /* discriminant */
    double d = k2 * k2 - 4 * k1 * k3;
    if (d < 0)
        return -1;

    t1 = (-k2 + sqrt(d)) / (2 * k1);
    t2 = (-k2 - sqrt(d)) / (2 * k1);
    t = t1 < t2? t1: t2;
    return t > EPSILON? t: -1;
}

/*
 * Finds the intersection between a ray and a triangle using the Moller-Trumbore algorithm.
 *
 * Sets intersection_point as the point of intersection, and returns the distance.
 * If the ray does not intersect the triangle, returns a negative number.
 */
double ray_intersects_polygon(double *ray_orig,
                              double *ray_dir,
                              struct matrix *polygons, int i)
{
    double v0v1[3], v0v2[3];
    double x0, y0, z0, x1, y1, z1, x2, y2, z2;
    double *p = &mt_idx(polygons, 0, i);
    x0 = *(p++); y0 = *(p++); z0 = *(p++); p++;
    x1 = *(p++); y1 = *(p++); z1 = *(p++); p++;
    x2 = *(p++); y2 = *(p++); z2 = *(p++);

    /* get edge vectors */
    v0v1[0] = x1 - x0; v0v1[1] = y1 - y0; v0v1[2] = z1 - z0;
    v0v2[0] = x2 - x0; v0v2[1] = y2 - y0; v0v2[2] = z2 - z0;

    double h[3], q[3];
    cross_product(ray_dir, v0v2, h);

    double det = dot_product(v0v1, h);
    if (-EPSILON < det && det < EPSILON) // ray is parallel
        return -1;

    /* floating point divisions are expensive :c */
    double inv_det = 1 / det;
    double s[3];
    s[0] = ray_orig[0] - x0;
    s[1] = ray_orig[1] - y0;
    s[2] = ray_orig[2] - z0;

    double u = dot_product(s, h) * inv_det;
    if (u < 0 || u > 1)
        return -1;

    cross_product(s, v0v1, q);
    double v = dot_product(ray_dir, q) * inv_det;
    if (1.0 < u + v || v < 0.0)
        return -1;

    double t = dot_product(v0v2, q) * inv_det;
    return t > EPSILON? t: -1;
}

/*
 * Trace a ray, return the color found.
 */
color trace_ray(double *ray_orig,
                double *ray_dir,
                struct object *objs,
                struct light **lights,
                color a_light,
                int depth)
{
    color background;
    background.r = 0;
    background.g = 180;
    background.b = 220;
    double min_dist = 1e8;
    double min_idx = -1;
    double intersection_point[3];
    struct object *min_object = NULL;

    /* find the closest object that intersects the ray */
    struct object *p = objs;
    while (p)
    {
        if (p->type == MESH_OBJ)
        {
            struct matrix *polygons = p->polygons;
            for (int i = 0; i < polygons->lastcol; i += 3)
            {
                double dist = ray_intersects_polygon(ray_orig, ray_dir, polygons, i);
                if (dist < min_dist && dist > 0)
                {
                    min_dist = dist;
                    min_idx = i;
                    min_object = p;
                }
            }
        }
        else if (p->type == SPHERE_OBJ)
        {
            double dist = ray_intersects_sphere(ray_orig, ray_dir, p->x, p->y, p->z, p->r);
            if (dist < min_dist && dist > 0)
            {
                min_dist = dist;
                min_object = p;
            }
        }
        p = p->next;
    }

    /* no object, return background color */
    if (min_object == NULL)
        return background;

    /* calculate intersection: P = O + Dt */
    intersection_point[0] = ray_orig[0] + min_dist * ray_dir[0];
    intersection_point[1] = ray_orig[1] + min_dist * ray_dir[1];
    intersection_point[2] = ray_orig[2] + min_dist * ray_dir[2];

    /* calculate standard phong lighting of the pixel */
    color il;
    int r, g, b;
    struct constants *reflect = min_object->constants;

    color a = calculate_ambient(a_light, reflect);
    r = a.r + reflect->red;
    g = a.g + reflect->green;
    b = a.b + reflect->blue;

    /* get normals */
    double normal[3], view[3] = {0, 0, 1};
    if (min_object->type == MESH_OBJ)
        calculate_normal(min_object->polygons, min_idx, normal);
    else if (min_object->type == SPHERE_OBJ)
    {
        normal[0] = intersection_point[0] - min_object->x;
        normal[1] = intersection_point[1] - min_object->y;
        normal[2] = intersection_point[2] - min_object->z;
    }

    struct light *light;
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        light = lights[i];
        if (!light)
            break;

        /* get ray to light */
        double l[3];
        /* TODO: I'm probably sure this isn't correct */
        l[0] = ray_orig[0] + XRES * light->l[0] - intersection_point[0];
        l[1] = ray_orig[1] + YRES * light->l[1] - intersection_point[1];
        l[2] = ray_orig[2] + light->l[2] - intersection_point[2];

        /* everything breaks if i dont normalize this vector why does this happen */
        normalize(l);
        
        /* check if any objects block the path to the light */
        p = objs;
        while (p)
        {
            if (p->type == MESH_OBJ)
            {
                struct matrix *polygons = p->polygons;
                for (int i = 0; i < polygons->lastcol; i += 3)
                {
                    double dist = ray_intersects_polygon(intersection_point, l, polygons, i);
                    if (dist > 0) goto end;
                }
            }
            else if (p->type == SPHERE_OBJ)
            {
                double dist = ray_intersects_sphere(intersection_point, l, p->x, p->y, p->z, p->r);
                if (dist > 0) goto end;
            }
            p = p->next;
        }

        /* if ray to light does not intersect any objects, add the illumination from the light */
        color d = calculate_diffuse(*light, reflect, normal);
        color s = calculate_specular(*light, reflect, view, normal);

        r += d.r + s.r;
        g += d.g + s.g;
        b += d.b + s.b;
end: {}
    }

    if (depth > 0)
    {
        /* set ray_dir to the direction of the reflection ray */
        double temp = dot_product(ray_dir, normal);
        ray_dir[0] -= 2 * normal[0] * temp;
        ray_dir[1] -= 2 * normal[1] * temp;
        ray_dir[2] -= 2 * normal[2] * temp;

        /* get the color from reflection */
        color reflected = trace_ray(intersection_point, ray_dir, objs, lights, a_light, depth - 1);

        /* constant for how reflective the surface is */
        /* this should probably be a variable in the constants of the object */
        double rf = 0.4;
        r *= 1 - rf;
        r += reflected.r * rf;
        g *= 1 - rf;
        g += reflected.g * rf;
        b *= 1 - rf;
        b += reflected.b * rf;
    }

    /* clamp to 8bit int */
    il.r = r < MAX_COLOR? r: MAX_COLOR;
    il.g = g < MAX_COLOR? g: MAX_COLOR;
    il.b = b < MAX_COLOR? b: MAX_COLOR;

    return il;
}

/*
 * Render all the objects!
 */
void render(Image s, struct object *objs, struct light **lights, color a_light)
{
    double ray_orig[3];
    ray_orig[0] = XRES / 2;
    ray_orig[1] = YRES / 2;
    ray_orig[2] = 500; // is this okay??

    for (int x = 0; x < XRES; ++x)
    {
        for (int y = 0; y < YRES; ++y)
        {
            printf("X: %3d, Y: %3d\r", x, y);
            fflush(stdout);

            double prim_ray[3];
            prim_ray[0] = (x + 0.5) - ray_orig[0];
            /* I'm not really sure why this has to be flipped */
            prim_ray[1] = ray_orig[1] - (y + 0.5);
            /* z of the image is zero */
            prim_ray[2] = -ray_orig[2];
            normalize(prim_ray);
            color c = trace_ray(ray_orig, prim_ray, objs, lights, a_light, 5);
            s[x][y] = c;
        }
    }
}

