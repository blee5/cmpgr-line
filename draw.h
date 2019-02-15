#include "image.h"

#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"

void add_column(struct matrix *m, double a, double b, double c, double d);
void add_point(struct matrix *points, double x, double y, double z);
void add_edge(struct matrix *points,
              double x0, double y0, double z0,
              double x1, double y1, double z1);
void draw_lines(struct matrix *points, Image s, color c);

void plot(int x, int y, Image s, color c);
void draw_line(int x0, int y0, int x1, int y1, Image s, color c);

#endif
