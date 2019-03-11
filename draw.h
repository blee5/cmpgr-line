#include "image.h"

#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"

/*
 * curve types
 */
#define HERMITE 0
#define BEZIER 1

void add_cubic_curve(struct matrix *points, double ax, double bx, double cx, double dx,
                                            double ay, double by, double cy, double dy, double step);
void add_bezier(struct matrix *points, double x0, double y0, double x1, double y1,
                                       double x2, double y2, double x3, double y3, double step);
void add_hermite(struct matrix *points, double x0, double y0, double x1, double y1,
                                        double x2, double y2, double x3, double y3, double step);

void add_circle(struct matrix *points, double cx, double cy, double cz, double r, double step);

void add_column(struct matrix *m, double a, double b, double c, double d);
void add_point(struct matrix *points, double x, double y, double z);
void add_edge(struct matrix *points,
              double x0, double y0, double z0,
              double x1, double y1, double z1);
void draw_lines(struct matrix *points, Image s, color c);

void plot(int x, int y, Image s, color c);
void draw_line(int x0, int y0, int x1, int y1, Image s, color c);

#endif
