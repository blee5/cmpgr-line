#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "gmath.h"

/*
 * Fills the ith polygon with horizontal lines
 */
void scanline_convert(struct matrix *polygons, int i, Image s, zbuffer zb, color il)
{
    double x0, y0, z0, x1, y1, z1, x2, y2, z2;
    double tx, tz, mx, mz, bx, bz;
    /* y values are int (important) */
    int ty, my, by;
    double *p;

    /* Unnecessarily obtuse code for reading three points */
    p = &mt_idx(polygons, 0, i * 3);
    x0 = *(p++); y0 = *(p++); z0 = *(p++); p++;
    x1 = *(p++); y1 = *(p++); z1 = *(p++); p++;
    x2 = *(p++); y2 = *(p++); z2 = *(p++);

    /* Sort points */
    if (y0 >= y1 && y0 >= y2)
    {
        tx = x0; ty = y0; tz = z0;
        /* P0 >= P1 >= P2 */
        if (y1 > y2)
        {
            mx = x1; my = y1; mz = z1;
            bx = x2; by = y2; bz = z2;
        }
        /* P0 >= P2 >= P1 */
        else
        {
            mx = x2; my = y2; mz = z2;
            bx = x1; by = y1; bz = z1;
        }
    }
    else if (y1 >= y0 && y1 >= y2)
    {
        tx = x1; ty = y1; tz = z1;
        /* P1 >= P0 >= P2 */
        if (y0 > y2)
        {
            mx = x0; my = y0; mz = z0;
            bx = x2; by = y2; bz = z2;
        }
        /* P1 >= P2 >= P0 */
        else
        {
            mx = x2; my = y2; mz = z2;
            bx = x0; by = y0; bz = z0;
        }
    }
    else
    {
        tx = x2; ty = y2; tz = z2;
        /* P2 >= P0 >= P1 */
        if (y0 > y1)
        {
            mx = x0; my = y0; mz = z0;
            bx = x1; by = y1; bz = z1;
        }
        /* P2 >= P1 >= P0 */
        else
        {
            mx = x1; my = y1; mz = z1;
            bx = x0; by = y0; bz = z0;
        }
    }

    double dx0 = ty - by? (tx - bx) / (ty - by): 0;
    double dz0 = ty - by? (tz - bz) / (ty - by): 0;
    double dx1 = my - by? (mx - bx) / (my - by): 0;
    double dz1 = my - by? (mz - bz) / (my - by): 0;
    x0 = x1 = bx;
    z0 = z1 = bz;

    for (int y = by; y <= ty; y++)
    {
        if (y == my)
        {
            dx1 = ty - my? (tx - mx) / (ty - my): 0;
            dz1 = ty - my? (tz - mz) / (ty - my): 0;
            x1 = mx; z1 = mz;
        }
        draw_line(x0, y, z0, x1, y, z1, s, zb, il);
        x0 += dx0; x1 += dx1;
        z0 += dz0; z1 += dz1;
    }
}

/*
 * Adds a sphere centered at (cx, cy, cz) with radius r
 */
void add_sphere(struct matrix *polygons, double cx, double cy, double cz, double r, int step)
{
    struct matrix *points = generate_sphere(cx, cy, cz, r, step);
    int lat, lgt;
    int p0, p1, p2, p3;
    for (lat = 0; lat < step; lat++)
    {
        for (lgt = 0; lgt < step; lgt++)
        {
            p0 = lat * (step + 1) + lgt;
            p1 = p0 + 1;
            p2 = (p1 + step) % points->lastcol;
            /* If drawing the polygon for the first pole, shift the vertex otherwise p2 == p0 */
            if (lgt == 0)
                p2++;
            add_polygon(polygons, mt_idx(points, 0, p0), mt_idx(points, 1, p0), mt_idx(points, 2, p0),
                                  mt_idx(points, 0, p1), mt_idx(points, 1, p1), mt_idx(points, 2, p1),
                                  mt_idx(points, 0, p2), mt_idx(points, 1, p2), mt_idx(points, 2, p2));
            /* Draw another triangle to make a quadrilateral, if we aren't at a pole */
            if (lgt > 0 && lgt < step - 1)
            {
                p3 = p2 + 1;
                add_polygon(polygons, mt_idx(points, 0, p1), mt_idx(points, 1, p1), mt_idx(points, 2, p1),
                                      mt_idx(points, 0, p3), mt_idx(points, 1, p3), mt_idx(points, 2, p3),
                                      mt_idx(points, 0, p2), mt_idx(points, 1, p2), mt_idx(points, 2, p2));
            }
        }
    }
    free_matrix(points);
}

/*
 * Helper function to generate points on a sphere, with (step + 1) points per semicircle
 */
struct matrix *generate_sphere(double cx, double cy, double cz, double r, int step)
{
    struct matrix *points = new_matrix(4, step * step);
    int rotation, circle;
    double phi, theta;
    double x, y, z;
    for (rotation = 0; rotation < step; rotation++)
    {
        phi = 2 * M_PI * (double)rotation / step;
        for (circle = 0; circle <= step; circle++)
        {
            theta = M_PI * (double)circle / step;
            x = r * cos(theta) + cx;
            y = r * sin(theta) * cos(phi) + cy;
            z = r * sin(theta) * sin(phi) + cz;
            add_point(points, x, y, z);
        }
    }
    return points;
}

/*
 * Adds a torus centered at (cx, cy, cz) with radius R,
 * with each cross section having radius r
 */
void add_torus(struct matrix *polygons, double cx, double cy, double cz, double r, double R, int step)
{
    struct matrix *points = generate_torus(cx, cy, cz, r, R, step);
    int lat, lgt;
    int p0, p1, p2, p3;
    for (lat = 0; lat < step; lat++)
    {
        for (lgt = 0; lgt < step; lgt++)
        {
            p0 = lat * (step + 1) + lgt;
            p1 = p0 + 1;
            p2 = (p1 + step) % points->lastcol;
            p3 = p2 + 1;
            add_polygon(polygons, mt_idx(points, 0, p0), mt_idx(points, 1, p0), mt_idx(points, 2, p0),
                                  mt_idx(points, 0, p1), mt_idx(points, 1, p1), mt_idx(points, 2, p1),
                                  mt_idx(points, 0, p2), mt_idx(points, 1, p2), mt_idx(points, 2, p2));
            add_polygon(polygons, mt_idx(points, 0, p1), mt_idx(points, 1, p1), mt_idx(points, 2, p1),
                                  mt_idx(points, 0, p3), mt_idx(points, 1, p3), mt_idx(points, 2, p3),
                                  mt_idx(points, 0, p2), mt_idx(points, 1, p2), mt_idx(points, 2, p2));
        }
    }
    free_matrix(points);
}

/*
 * Helper function to generate the points for a torus, with each cross section containing step + 1 points
 * ([step] points for the circle, and an extra point to return back to the beginning point)
 */
struct matrix *generate_torus(double cx, double cy, double cz, double r, double R, int step)
{
    struct matrix *points = new_matrix(4, step * step);
    int rotation, circle;
    double phi, theta;
    double x, y, z;
    for (rotation = 0; rotation < step; rotation++)
    {
        phi = 2 * M_PI * (double)rotation / step;
        for (circle = 0; circle <= step; circle++)
        {
            theta = 2 * M_PI * (double)circle / step;
            x = (r * cos(theta) + R) * cos(phi) + cx;
            y = r * sin(theta) + cy;
            z = (r * cos(theta) + R) * sin(phi) + cz;
            add_point(points, x, y, z);
        }
    }
    return points;
}

/*
 * Adds a box with the top left vertex located at (x, y, z)
 * with width dx, height dy, and depth dz
 */
void add_box(struct matrix *polygons, double x, double y, double z, double dx, double dy, double dz)
{
    double x0 = x, y0 = y, z0 = z;
    double x1 = x + dx, y1 = y - dy, z1 = z - dz;
    add_polygon(polygons, x0, y0, z0, x0, y1, z0, x1, y0, z0);
    add_polygon(polygons, x1, y0, z0, x0, y1, z0, x1, y1, z0);
    add_polygon(polygons, x0, y0, z1, x1, y0, z1, x0, y1, z1);
    add_polygon(polygons, x1, y0, z1, x1, y1, z1, x0, y1, z1);

    add_polygon(polygons, x0, y0, z0, x1, y0, z0, x1, y0, z1);
    add_polygon(polygons, x1, y0, z1, x0, y0, z1, x0, y0, z0);
    add_polygon(polygons, x0, y1, z0, x1, y1, z1, x1, y1, z0);
    add_polygon(polygons, x1, y1, z1, x0, y1, z0, x0, y1, z1);
    
    add_polygon(polygons, x0, y0, z0, x0, y1, z1, x0, y1, z0);
    add_polygon(polygons, x0, y0, z0, x0, y0, z1, x0, y1, z1);
    add_polygon(polygons, x1, y0, z0, x1, y1, z0, x1, y1, z1);
    add_polygon(polygons, x1, y0, z0, x1, y1, z1, x1, y0, z1);
}

/*
 * Adds a circle centered at (cx, cy, cz) with radius r
 */
void add_circle(struct matrix *edges, double cx, double cy, double cz, double r, double step)
{
    double x0, y0, x1, y1;
    double t = 0, theta = 0;
    while (t < 1)
    {
        theta = t * M_PI * 2;
        x0 = cx + r * cos(theta);
        y0 = cy + r * sin(theta);
        t += step;
        theta = t * M_PI * 2;
        x1 = cx + r * cos(theta);
        y1 = cy + r * sin(theta);
        add_edge(edges, x0, y0, cz, x1, y1, cz);
    }
}

/*
 * Draws a cubic curve defined parametrically in the form:
 *     x = ax t^3 + bx t^2 + cx t + dx
 *     y = ay t^3 + bx t^2 + cx t + dx
 */
void add_cubic_curve(struct matrix *edges, double ax, double bx, double cx, double dx,
                                            double ay, double by, double cy, double dy, double step)
{
    double x0, y0, x1, y1, t = 0;
    x0 = (ax * t * t * t) + (bx * t * t) + cx * t + dx;
    y0 = (ay * t * t * t) + (by * t * t) + cy * t + dy;
    while (t < 1)
    {
        t += step;
        x1 = (ax * t * t * t) + (bx * t * t) + cx * t + dx;
        y1 = (ay * t * t * t) + (by * t * t) + cy * t + dy;
        add_edge(edges, x0, y0, 0, x1, y1, 0);
        x0 = x1;
        y0 = y1;
    }
}

/*
 * Adds a cubic Bezier curve defined with given points, where (x0, y0) and (x3, y3) are endpoints
 */
void add_bezier(struct matrix *edges, double x0, double y0, double x1, double y1,
                                       double x2, double y2, double x3, double y3, double step)
{
    /* Parametric equation coefficients */
    double ax = -x0 + 3 * x1 - 3 * x2 + x3;
    double bx = 3 * x0 - 6 * x1 + 3 * x2;
    double cx = -3 * x0 + 3 * x1;
    double dx = x0;
    double ay = -y0 + 3 * y1 - 3 * y2 + y3;
    double by = 3 * y0 - 6 * y1 + 3 * y2;
    double cy = -3 * y0 + 3 * y1;
    double dy = y0;
    add_cubic_curve(edges, ax, bx, cx, dx, ay, by, cy, dy, step);
}

/*
 * Adds a Hermite curve defined with given points, where (x0, y0) and (x1, y1) are endpoints
 */
void add_hermite(struct matrix *edges, double x0, double y0, double x1, double y1,
                                        double x2, double y2, double x3, double y3, double step)
{
    /* Parametric equation coefficients */
    double ax = 2 * x0 + -2 * x1 + x2 + x3;
    double bx = -3 * x0 + 3 * x1 + -2 * x2 + -x3;
    double cx = x2;
    double dx = x0;
    double ay = 2 * y0 + -2 * y1 + y2 + y3;
    double by = -3 * y0 + 3 * y1 + -2 * y2 + -y3;
    double cy = y2;
    double dy = y0;
    add_cubic_curve(edges, ax, bx, cx, dx, ay, by, cy, dy, step);
}

/*
 * Adds point (x, y, z) to the matrix.
 * The matrix MUST have exactly 4 rows.
 */
void add_point(struct matrix *m, double x, double y, double z)
{
    if (m->lastcol == m->cols)
    {
        grow_matrix(m, 100);
    }
    double *p = &mt_idx(m, 0, m->lastcol);
    m->lastcol++;
    *p++ = x; *p++ = y; *p++ = z; *p = 1;
}

/*
 * Adds a line connecting (x0, y0, z0) to (x1, y1, z1) to edges.
 * The matrix MUST have exactly 4 rows.
 */
void add_edge(struct matrix *edges,
              double x0, double y0, double z0,
              double x1, double y1, double z1)
{
    add_point(edges, x0, y0, z0);
    add_point(edges, x1, y1, z1);
}

/*
 * Add a polygon to the polygon matrix
 */
void add_polygon(struct matrix *polygons,
                 double x0, double y0, double z0,
                 double x1, double y1, double z1,
                 double x2, double y2, double z2)
{
    add_point(polygons, x0, y0, z0);
    add_point(polygons, x1, y1, z1);
    add_point(polygons, x2, y2, z2);
}

/*
 * Goes through edges 2 at a time and draws lines connecting the edges.
 * The matrix should obviously have an even number of columns.
 */
void draw_edges(struct matrix *edges, Image s, zbuffer zb, color c)
{
    int col;
    int x0, y0, z0, x1, y1, z1;
    for (col = 0; col < edges->lastcol; col += 2)
    {
        x0 = mt_idx(edges, 0, col);
        y0 = mt_idx(edges, 1, col);
        z0 = mt_idx(edges, 2, col);
        x1 = mt_idx(edges, 0, col + 1);
        y1 = mt_idx(edges, 1, col + 1);
        z1 = mt_idx(edges, 2, col + 1);
        draw_line(x0, y0, z0, x1, y1, z1, s, zb, c);
    }
}

/*
 * Draw the polygons from the polygon matrix
 */
void draw_polygons(struct matrix *polygons, Image s, zbuffer zb,
                   double *view, struct light *lights[], color ambient,
                   struct constants constants)
{
    int col;
    double normal[3];
    for (col = 0; col < polygons->lastcol; col += 3)
    {
        /* Backface culling */
        calculate_normal(polygons, col, normal);
        if (dot_product(view, normal) > 0)
        {
            color i = get_lighting(normal, view, ambient, lights, &constants);
            
            scanline_convert(polygons, col / 3, s, zb, i);
        }
    }
}

void draw_line(int x0, int y0, double z0,
               int x1, int y1, double z1, Image s, zbuffer zb, color c)
{
    int x, y;
    int dx, dy;
    double z, dz;
    /* D = Ax + By + C, where A = dy, B = -dx, and C is irrelevant */
    int A, B, D;
    /* Swap points to order them */
    if (x1 < x0)
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = z0;
        z0 = z1;
        z1 = temp;
    }
    x = x0;
    y = y0;
    z = z0;
    dx = x1 - x0;
    dy = y1 - y0;
    dz = z1 - z0;

    /* Note everything is scaled by 2 to avoid the pesky floating point number 1/2 */
    A = dy * 2;
    B = -dx * 2;
    if (abs(dx) >= abs(dy)) // -1 <= m <= 1
    {
        int cy = 1;
        if (dy < 0)
        {
            cy = -1;
            A *= -1;
        }
        D = 2 * A + B;
        while (x <= x1)
        {
            plot(x, y, z, s, zb, c);
            if (D > 0) // next midpoint is below the line
            {
                y += cy;
                D += B;
            }
            z += dz / dx;
            x++;
            D += A;
        }
    }
    else
    {
        if (dy >= 0)
        {
            D = A + 2 * B;
            while (y <= y1)
            {
                plot(x, y, z, s, zb, c);
                if (D < 0) // next midpoint is above the line
                {
                    x++;
                    D += A;
                }
                z += dz / dy;
                y++;
                D += B;
            }
        }
        else
        {
            D = A - 2 * B;
            while (y >= y1)
            {
                plot(x, y, z, s, zb, c);
                if (D > 0) // next midpoint is below the line
                {
                    x++;
                    D += A;
                }
                z += dz / dy;
                y--;
                D -= B;
            }
        }
    }
}

void plot(int x, int y, double z, Image s, zbuffer zb, color c)
{
    /* NOTE: (0, 0) is the bottom left corner */
    y = YRES - 1 - y;
    if (x >= 0 && x < XRES && y >= 0 && y < YRES)
    {
        if (zb[x][y] < z)
        {
            s[x][y] = c;
            /* Subtracting a very small number seems to reduce the number of blurry edges
             * between polygons with very similar z-values, but not sure if this is a good idea */
            zb[x][y] = z - 0.00001;
            /* zb[x][y] = z; */
        }
    }
}

