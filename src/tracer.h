#ifndef TRACER_H
#define TRACER_H

#include "matrix.h"
#include "image.h"
#include "symtab.h"

struct object
{
    struct matrix *polygons;
    struct constants *constants;
    struct object *next;
};

void add_object(struct object **objs, struct matrix *polygons, struct constants *constants);
void render(Image s, struct object *objs, struct light **lights, color a_light);

#endif

