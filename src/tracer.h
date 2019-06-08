#ifndef TRACER_H
#define TRACER_H

#include "matrix.h"
#include "image.h"
#include "symtab.h"

void ray_trace(Image s, struct matrix *polygons, struct light **lights, double *view);

#endif

