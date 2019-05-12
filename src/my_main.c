#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "gmath.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "stack.h"
#include "parser.h"
#include "../yacc/mdl.tab.h"

void my_main()
{
    color ambient;
    double light[2][3];
    double view[3];

    /* default values */
    NUM_POLY = 1000;
    SHINYNESS = 30;
    ambient.r = 200;
    ambient.g = 200;
    ambient.b = 200;

    light[LOCATION][0] = 1;
    light[LOCATION][1] = 1;
    light[LOCATION][2] = 1;
    light[COLOR][RED] = 200;
    light[COLOR][GREEN] = 200;
    light[COLOR][BLUE] = 200;

    view[0] = 0;
    view[1] = 0;
    view[2] = 1;

    struct constants white;
    white.r[AMBIENT_R] = 0.3;
    white.g[AMBIENT_R] = 0.3;
    white.b[AMBIENT_R] = 0.3;

    white.r[DIFFUSE_R] = 0.9;
    white.g[DIFFUSE_R] = 0.9;
    white.b[DIFFUSE_R] = 0.9;

    white.r[SPECULAR_R] = 0.4;
    white.g[SPECULAR_R] = 0.4;
    white.b[SPECULAR_R] = 0.4;

    /* initialize image */
    Image *s = init_image();
    zbuffer *zb = init_zbuffer();

    /* initialize matrices for storing points */
    struct matrix *edges, *polygons;
    edges = new_matrix(4, 0);
    polygons = new_matrix(4, 0);

    /* initialize stack for storing coordinate systems */
    struct stack *stack = new_stack();

    struct matrix *transform;
    for (int i = 0; i < lastop; ++i)
    {
        struct constants *constants = &white;
        struct matrix *cs = peek(stack);
        switch (op[i].opcode)
        {
            /*
             * 3d solids
             */
            case SPHERE:
            {
                double x, y, z, r;
                x = op[i].op.sphere.d[0];
                y = op[i].op.sphere.d[1];
                z = op[i].op.sphere.d[2];
                r = op[i].op.sphere.r;
                if (op[i].op.sphere.constants)
                    constants = op[i].op.sphere.constants->s.c;
                if (op[i].op.sphere.cs)
                    cs = op[i].op.sphere.cs->s.m;
                add_sphere(polygons, x, y, z, r, NUM_POLY);
                matrix_mult(cs, polygons);
                draw_polygons(polygons, *s, *zb,
                              view, light, ambient, *constants);
                polygons->lastcol = 0;
                break;
            }
            case BOX:
            {
                double x, y, z, h, w, d;
                x = op[i].op.box.d0[0];
                y = op[i].op.box.d0[1];
                z = op[i].op.box.d0[2];
                h = op[i].op.box.d1[0];
                w = op[i].op.box.d1[1];
                d = op[i].op.box.d1[2];
                if (op[i].op.box.constants)
                    constants = op[i].op.box.constants->s.c;
                if (op[i].op.box.cs)
                    cs = op[i].op.box.cs->s.m;
                add_box(polygons, x, y, z, h, w, d);
                matrix_mult(cs, polygons);
                draw_polygons(polygons, *s, *zb,
                              view, light, ambient, *constants);
                polygons->lastcol = 0;
                break;
            }
            case TORUS:
            {
                double x0, y0, z0, r0, r1;
                x0 = op[i].op.torus.d[0];
                y0 = op[i].op.torus.d[1];
                z0 = op[i].op.torus.d[2];
                r0 = op[i].op.torus.r0;
                r1 = op[i].op.torus.r1;
                if (op[i].op.torus.constants)
                    constants = op[i].op.torus.constants->s.c;
                if (op[i].op.torus.cs)
                    cs = op[i].op.torus.cs->s.m;
                add_torus(polygons, x0, y0, z0, r0, r1, NUM_POLY);
                matrix_mult(cs, polygons);
                draw_polygons(polygons, *s, *zb,
                              view, light, ambient, *constants);
                polygons->lastcol = 0;
                break;
            }

            /*
             * transformations
             */
            case SCALE:
            {
                double x, y, z;
                x = op[i].op.scale.d[0];
                y = op[i].op.scale.d[1];
                z = op[i].op.scale.d[2];
                transform = make_scale(x, y, z);
                matrix_mult(transform, peek(stack));
                free_matrix(transform);
                break;
            }
            case MOVE:
            {
                double x, y, z;
                x = op[i].op.move.d[0];
                y = op[i].op.move.d[1];
                z = op[i].op.move.d[2];
                transform = make_translate(x, y, z);
                matrix_mult(transform, peek(stack));
                free_matrix(transform);
                break;
            }
            case ROTATE:
            {
                double theta = op[i].op.rotate.degrees;
                switch ((int)op[i].op.rotate.axis)
                {
                    case 0:
                        transform = make_rotX(theta);
                        break;
                    case 1:
                        transform = make_rotY(theta);
                        break;
                    default:
                        transform = make_rotZ(theta);
                        break;
                }
                matrix_mult(transform, peek(stack));
                free_matrix(transform);
                break;
            }

            /*
             * stack operations
             */
            case PUSH:
            {
                transform = new_matrix(4, 4);
                copy_matrix(peek(stack), transform);
                push(stack, transform);
                break;
            }
            case POP:
            {
                pop(stack);
                break;
            }
            case SAVE_COORDS:
            {
                copy_matrix(peek(stack), op[i].op.save_coordinate_system.p->s.m);
                break;
            }

            /*
             * misc
             */
            case SAVE:
            {
                save_image(*s, op[i].op.save.p->name);
                break;
            }
            case DISPLAY:
            {
                display(*s);
                break;
            }
        }
    }
    /* print_symtab(); */

    free(zb);
    free(s);
    free_matrix(edges);
    free_matrix(polygons);
}
