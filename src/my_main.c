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
#include "mdl.tab.h"

void first_pass()
{
    extern int num_frames;
    extern char *name;
    num_frames = -1;
    name = NULL;

    int is_anim = 0;
    
    for (int i = 0; i < lastop; ++i)
    {
        switch (op[i].opcode)
        {
            case VARY:
            {
                is_anim = 1;
                break;
            }
            case FRAMES:
            {
                num_frames = op[i].op.frames.num_frames;
                break;
            }
            case BASENAME:
            {
                name = op[i].op.basename.p->name;
                break;
            }
        }
    }
    if (is_anim && name == NULL)
    {
        fprintf(stderr, "Warning: basename not found, using default value of 'foo'\n");
        name = "foo";
    }

    if (is_anim && num_frames < 0)
    {
        fprintf(stderr, "Number of frames not set\n");
        exit(-1);
    }
}

/*======== struct vary_node ** second_pass() ==========
    Returns: An array of vary_node linked lists

    In order to set the knobs for animation, we need to keep
    a seaprate value for each knob for each frame. We can do
    this by using an array of linked lists. Each array index
    will correspond to a frame (eg. knobs[0] would be the first
    frame, knobs[2] would be the 3rd frame and so on).

    Each index should contain a linked list of vary_nodes, each
    node contains a knob name, a value, and a pointer to the
    next node.

    Go through the opcode array, and when you find vary, go
    from knobs[0] to knobs[frames-1] and add (or modify) the
    vary_node corresponding to the given knob with the
    appropirate value.
    ====================*/
struct vary_node **second_pass()
{
    struct vary_node *curr = NULL;
    struct vary_node **knobs = calloc(num_frames, sizeof(struct vary_node *));

    for (int i = 0; i < lastop; ++i)
    {
        if (op[i].opcode == VARY)
        {
            int start = op[i].op.vary.start_frame;
            int end = op[i].op.vary.end_frame;
            double val_i = op[i].op.vary.start_val;
            double val_e = op[i].op.vary.end_val;
            double dv = (val_e - val_i) / (end - start);
            char *name = op[i].op.vary.p->name;

            double val = val_i;
            for (int frame = start; frame < end; ++frame)
            {
                struct vary_node *temp = malloc(sizeof(struct vary_node));
                strncpy(temp->name, name, 128);
                temp->value = val_i;
                val_i += dv;
                struct vary_node *it = knobs[frame];
                if (it)
                {
                    while (it->next) it = it->next;
                    it->next = temp;
                }
                else
                    knobs[frame] = temp;
            }
        }
    }
    return knobs;
}


void my_main()
{
    struct vary_node **knobs;
    struct vary_node *vn;
    first_pass();
    knobs = second_pass();
    char frame_name[128];
    int f;

    /* temp_line_colororary color for lines, replace this later */
    color temp_line_color;
    temp_line_color.r = 255;
    temp_line_color.g = 0;
    temp_line_color.b = 0;

    color ambient;
    double light[2][3];
    double view[3];

    /* default values */
    NUM_POLY = 500;
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
    struct constants *reflect = &white;
    white.r[AMBIENT_R] = 0.3;
    white.g[AMBIENT_R] = 0.3;
    white.b[AMBIENT_R] = 0.3;

    white.r[DIFFUSE_R] = 0.9;
    white.g[DIFFUSE_R] = 0.9;
    white.b[DIFFUSE_R] = 0.9;

    white.r[SPECULAR_R] = 0.4;
    white.g[SPECULAR_R] = 0.4;
    white.b[SPECULAR_R] = 0.4;

    int cur_frame = 0;
    do
    {
        sprintf(frame_name, "anim/%s_%03d.png", name, cur_frame);

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
            struct constants *constants = reflect;
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
                 * 2d objects
                 */
                case LINE:
                {
                    double x0, y0, z0, x1, y1, z1;
                    x0 = op[i].op.line.p0[0];
                    y0 = op[i].op.line.p0[1];
                    z0 = op[i].op.line.p0[0];
                    x1 = op[i].op.line.p1[0];
                    y1 = op[i].op.line.p1[1];
                    z1 = op[i].op.line.p1[2];
                    add_edge(edges, x0, y0, z0, x1, y1, z1);
                    matrix_mult(cs, edges);
                    draw_edges(edges, *s, *zb, temp_line_color);
                    edges->lastcol = 1;
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
                    matrix_mult(cs, transform);
                    copy_matrix(transform, cs);
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
                    matrix_mult(cs, transform);
                    copy_matrix(transform, cs);
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
                    matrix_mult(cs, transform);
                    copy_matrix(transform, cs);
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

        if (num_frames > 0) // is animation
        {
            save_image(*s, frame_name);
            printf("%s\n", frame_name);
        }

        free(zb);
        free(s);
        free_matrix(edges);
        free_matrix(polygons);
    }
    while (cur_frame++ < num_frames);
}
