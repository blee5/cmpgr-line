#include "image.h"

#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"

void scanline_convert(struct matrix *polygons, int i, Image s, zbuffer zb, color il);

void add_sphere(struct matrix *polygons, double cx, double cy, double cz, double r, int step);
struct matrix *generate_sphere(double cx, double cy, double cz, double r, int step);

void add_torus(struct matrix *polygons, double cx, double cy, double cz, double r, double R, int step);
struct matrix *generate_torus(double cx, double cy, double cz, double r, double R, int step);

void add_box(struct matrix *polygons, double x, double y, double z, double dx, double dy, double dz);
void add_cubic_curve(struct matrix *edges, double ax, double bx, double cx, double dx,
                                           double ay, double by, double cy, double dy, double step);
void add_bezier(struct matrix *edges, double x0, double y0, double x1, double y1,
                                      double x2, double y2, double x3, double y3, double step);
void add_hermite(struct matrix *edges, double x0, double y0, double x1, double y1,
                                       double x2, double y2, double x3, double y3, double step);
void add_circle(struct matrix *edges, double cx, double cy, double cz, double r, double step);

void add_point(struct matrix *edges, double x, double y, double z);
void add_edge(struct matrix *edges,
              double x0, double y0, double z0,
              double x1, double y1, double z1);
void add_polygon(struct matrix *polygons,
                 double x0, double y0, double z0,
                 double x1, double y1, double z1,
                 double x2, double y2, double z2);
void draw_edges(struct matrix *edges, Image s, zbuffer zb, color c);
void draw_polygons(struct matrix *polygons, Image s, zbuffer zb,
                   double *view, double light[2][3], color ambient,
                   double *a_reflect, double *d_reflect, double *s_reflect);

void plot(int x, int y, double z, Image s, zbuffer zb, color c);
void draw_line(int x0, int y0, double z0,
               int x1, int y1, double z1, Image s, zbuffer zb, color c);
#endif
