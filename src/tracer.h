#ifndef TRACER_H
#define TRACER_H

#include "matrix.h"
#include "image.h"
#include "symtab.h"

enum OBJ_TYPES
{
    MESH_OBJ,
    SPHERE_OBJ,
};

struct object
{
    union
    {
        struct matrix *polygons;
        struct
        {
            double x, y, z;
            double r;
        };
    };
    struct constants *constants;
    struct object *next;
    int type;
};

void add_object_sphere(struct object **objs, double x, double y, double z, double r, struct constants *constants);
void add_object(struct object **objs, struct matrix *polygons, struct constants *constants);
void render(Image s, struct object *objs, struct light **lights, color a_light);

#endif

