#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"

/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single word that takes up a line.
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing
====================*/
void parse_file(char *filename, 
                struct matrix *transform, 
                struct matrix *edges,
                Image s)
{
    FILE *f;
    char line[256];
    double x0, y0, z0, x1, y1, z1, x2, y2, x3, y3;
    double x, y, z;
    double r;
    double theta;
    char axis;

    color c;
    c.r = 0; c.g = 255; c.b = 100;

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
  
    clear_image(s);
    while (fgets(line, 255, f) != NULL)
    {
        line[strlen(line)-1] = '\0';
        if (strcmp(line, "line") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);
            add_edge(edges, x0, y0, z0, x1, y1, z1);
        }
        else if (strcmp(line, "circle") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf", &x, &y, &z, &r);
            add_circle(edges, x, y, z, r, 0.005);
        }
        else if (strcmp(line, "hermite") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                          &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
            add_hermite(edges, x0, y0, x1, y1, x2, y2, x3, y3, 0.005);
        }
        else if (strcmp(line, "bezier") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                          &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
            add_bezier(edges, x0, y0, x1, y1, x2, y2, x3, y3, 0.005);
        }
        else if (strcmp(line, "translate") == 0 || strcmp(line, "move") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &x, &y, &z);
            make_translate(transform, x, y, z);
        }
        else if (strcmp(line, "scale") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &x, &y, &z);
            make_scale(transform, x, y, z);
        }
        else if (strcmp(line, "rotate") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%c %lf", &axis, &theta);
            switch (axis)
            {
                case 'x':
                    make_rotX(transform, theta);
                    break;
                case 'y':
                    make_rotY(transform, theta);
                    break;
                case 'z':
                    make_rotZ(transform, theta);
                    break;
            }
        }
        else if (strcmp(line, "apply") == 0)
        {
            matrix_mult(transform, edges);
        }
        else if (strcmp(line, "ident") == 0)
        {
            free_matrix(transform);
            transform = ident(4);
        }
        else if (strcmp(line, "display") == 0)
        {
            clear_image(s);
            draw_lines(edges, s, c);
            display(s);
        }
        else if (strcmp(line, "save") == 0)
        {
            fgets(line, 255, f);
            clear_image(s);
            draw_lines(edges, s, c);
            save_image(s, line);
        }
        else if (strcmp(line, "quit") == 0)
        {
            free(s);
            exit(0);
        }
    }
}
  
