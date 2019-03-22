#ifndef PARSER_H
#define PARSER_H

#include "matrix.h"
#include "image.h"

void parse_file(char *filename,
                struct matrix *transform,
                struct matrix *edges,
                struct matrix *polygons,
                Image s);

#endif
