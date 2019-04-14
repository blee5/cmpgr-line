#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"
#include "stack.h"

/*======== void parse_file () ==========
Inputs:   char *filename
          struct matrix *edges,
          struct matrix *polygons,
          screen s

Goes through the file named filename and performs all of the actions listed in that file.
====================*/
void parse_file(char *filename, 
                struct matrix *edges,
                struct matrix *polygons,
                Image s,
                zbuffer zb)
{
    FILE *f;
    char line[256];
    struct stack *stack = new_stack();
    struct matrix *transform;

    double x0, y0, z0, x1, y1, z1, x2, y2, x3, y3;
    double r, R;
    double theta;
    char axis;

    color c;
    c.r = 0; c.g = 255; c.b = 200;

    f = stdin;
    if (strcmp(filename, "stdin") != 0)
    {
        f = fopen(filename, "r");
        if (f == NULL)
        {
            perror("Script could not be found");
            exit(-1);
        }
    }
  
    while (fgets(line, 255, f) != NULL)
    {
        line[strlen(line)-1] = '\0';
        if (strcmp(line, "push") == 0)
        {
            transform = new_matrix(4, 4);
            copy_matrix(peek(stack), transform);
            push(stack, transform);
        }
        else if (strcmp(line, "pop") == 0)
        {
            pop(stack);
        }
        else if (strcmp(line, "line") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);
            add_edge(edges, x0, y0, z0, x1, y1, z1);
            matrix_mult(peek(stack), edges);
            draw_edges(edges, s, zb, c);
            edges->lastcol = 0;
        }
        else if (strcmp(line, "circle") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf", &x0, &y0, &z0, &r);
            add_circle(edges, x0, y0, z0, r, 0.005);
            matrix_mult(peek(stack), edges);
            draw_edges(edges, s, zb, c);
            edges->lastcol = 0;
        }
        else if (strcmp(line, "bezier") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                          &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
            add_bezier(edges, x0, y0, x1, y1, x2, y2, x3, y3, 0.005);
            matrix_mult(peek(stack), edges);
            draw_edges(edges, s, zb, c);
            edges->lastcol = 0;
        }
        else if (strcmp(line, "hermite") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                          &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
            add_hermite(edges, x0, y0, x1, y1, x2, y2, x3, y3, 0.005);
            matrix_mult(peek(stack), edges);
            draw_edges(edges, s, zb, c);
            edges->lastcol = 0;
        }
        else if (strcmp(line, "box") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf",
                         &x0, &y0, &z0, &x1, &y1, &z1);
            add_box(polygons, x0, y0, z0, x1, y1, z1);
            matrix_mult(peek(stack), polygons);
            draw_polygons(polygons, s, zb, c);
            polygons->lastcol = 0;
        }
        else if (strcmp(line, "sphere") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf",
                         &x0, &y0, &z0, &r);
            add_sphere(polygons, x0, y0, z0, r, 2 * sqrt(r));
            matrix_mult(peek(stack), polygons);
            draw_polygons(polygons, s, zb, c);
            polygons->lastcol = 0;
        }
        else if (strcmp(line, "torus") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf",
                         &x0, &y0, &z0, &r, &R);
            add_torus(polygons, x0, y0, z0, r, R, 3 * sqrt(r));
            matrix_mult(peek(stack), polygons);
            draw_polygons(polygons, s, zb, c);
            polygons->lastcol = 0;
        }
        else if (strcmp(line, "translate") == 0 || strcmp(line, "move") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &x0, &y0, &z0);
            transform = make_translate(x0, y0, z0);
            matrix_mult(peek(stack), transform);
            copy_matrix(transform, peek(stack));
            free_matrix(transform);
        }
        else if (strcmp(line, "scale") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &x0, &y0, &z0);
            transform = make_scale(x0, y0, z0);
            matrix_mult(peek(stack), transform);
            copy_matrix(transform, peek(stack));
            free_matrix(transform);
        }
        else if (strcmp(line, "rotate") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%c %lf", &axis, &theta);
            switch (axis)
            {
                case 'x':
                    transform = make_rotX(theta);
                    break;
                case 'y':
                    transform = make_rotY(theta);
                    break;
                case 'z':
                    transform = make_rotZ(theta);
                    break;
            }
            matrix_mult(peek(stack), transform);
            copy_matrix(transform, peek(stack));
            free_matrix(transform);
        }
        else if (strcmp(line, "display") == 0)
        {
            display(s);
        }
        else if (strcmp(line, "clear") == 0)
        {
            clear_zbuffer(zb);
            clear_image(s);
        }
        else if (strcmp(line, "save") == 0)
        {
            fgets(line, 255, f);
            save_image(s, line);
        }
        else if (strcmp(line, "quit") == 0)
        {
            free_matrix(edges);
            free_matrix(polygons);
            free_stack(stack);
            free(s);
            exit(0);
        }
    }
}
  
