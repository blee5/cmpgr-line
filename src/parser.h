#ifndef PARSER_H
#define PARSER_H

#include "matrix.h"
#include "image.h"

void parse_file(char *filename, 
                struct matrix *edges,
                struct matrix *polygons,
                Image s, zbuffer zb,
                double *view, color ambient, double light[2][3],
                double *a_reflect, double *s_reflect, double *d_reflect);

#endif
