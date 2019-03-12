#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"

/*
 * Adds a circle centered at (cx, cy, cz) with radius r
 */
void add_circle(struct matrix *points, double cx, double cy, double cz, double r, double step)
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
        add_edge(points, x0, y0, cz, x1, y1, cz);
    }
}

/*
 * Draws a cubic curve defined parametrically in the form:
 *     x = ax t^3 + bx t^2 + cx t + dx
 *     y = ay t^3 + bx t^2 + cx t + dx
 */
void add_cubic_curve(struct matrix *points, double ax, double bx, double cx, double dx,
                                            double ay, double by, double cy, double dy, double step)
{
    double xp0, yp0, xp1, yp1, t = 0;
    while (t < 1)
    {
        xp0 = ax * pow(t, 3) + bx * pow(t, 2) + cx * t + dx;
        yp0 = ay * pow(t, 3) + by * pow(t, 2) + cy * t + dy;
        t += step;
        xp1 = ax * pow(t, 3) + bx * pow(t, 2) + cx * t + dx;
        yp1 = ay * pow(t, 3) + by * pow(t, 2) + cy * t + dy;
        add_edge(points, xp0, yp0, 0, xp1, yp1, 0);
    }
}

/*
 * Adds a cubic Bezier curve defined with given points, where (x0, y0) and (x3, y3) are endpoints
 */
void add_bezier(struct matrix *points, double x0, double y0, double x1, double y1,
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
    add_cubic_curve(points, ax, bx, cx, dx, ay, by, cy, dy, step);
}

/*
 * Adds a Hermite curve defined with given points, where (x0, y0) and (x1, y1) are endpoints
 */
void add_hermite(struct matrix *points, double x0, double y0, double x1, double y1,
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
    add_cubic_curve(points, ax, bx, cx, dx, ay, by, cy, dy, step);
}

/*
 * Adds a column to the matrix.
 * The matrix MUST have exactly 4 rows.
 */
void add_column(struct matrix *m, double a, double b, double c, double d)
{
    if (m->lastcol == m->cols)
    {
        grow_matrix(m, 100);
    }
    double *p = &mt_idx(m, 0, m->lastcol);
    m->lastcol++;
    *p++ = a; *p++ = b; *p++ = c; *p = d;
}

/*
 * Adds point (x, y, z) to the matrix.
 * The matrix MUST have exactly 4 rows.
 */
void add_point(struct matrix *m, double x, double y, double z)
{
    add_column(m, x, y, z, 1);
}

/*
 * Adds a line connecting (x0, y0, z0) to (x1, y1, z1) to points.
 * The matrix MUST have exactly 4 rows.
 */
void add_edge(struct matrix *points,
              double x0, double y0, double z0,
              double x1, double y1, double z1)
{
    add_point(points, x0, y0, z0);
    add_point(points, x1, y1, z1);
}

/*
 * Goes through points 2 at a time and draws lines connecting the points.
 * The matrix should obviously have an even number of columns.
 */
void draw_lines(struct matrix *points, Image s, color c)
{
    int col;
    int x0, y0, x1, y1;
    for (col = 0; col < points->lastcol; col += 2)
    {
        x0 = mt_idx(points, 0, col);
        y0 = mt_idx(points, 1, col);
        x1 = mt_idx(points, 0, col + 1);
        y1 = mt_idx(points, 1, col + 1);
        draw_line(x0, y0, x1, y1, s, c);
    }
}

void plot(int x, int y, Image s, color c)
{
    /* NOTE: (0, 0) is the bottom left corner */
    y = YRES - 1 - y;
    if (x >= 0 && x < XRES && y >= 0 && y < YRES)
    {
        s[x][y] = c;
    }
}

void draw_line(int x0, int y0, int x1, int y1, Image s, color c)
{
    int x, y;
    int dx, dy;
    /* D = Ax + By + C, where A = dy, B = -dx, and C is irrelevant */
    int A, B, D;
    if (x1 < x0)
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;

        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    x = x0;
    y = y0;
    dx = x1 - x0;
    dy = y1 - y0;

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
            plot(x, y, s, c);
            if (D > 0) // next midpoint is below the line
            {
                y += cy;
                D += B;
            }
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
                plot(x, y, s, c);
                if (D < 0) // next midpoint is above the line
                {
                    x++;
                    D += A;
                }
                y++;
                D += B;
            }
        }
        else
        {
            D = A - 2 * B;
            while (y >= y1)
            {
                plot(x, y, s, c);
                if (D > 0) // next midpoint is below the line
                {
                    x++;
                    D += A;
                }
                y--;
                D -= B;
            }
        }
    }
}
