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

struct vary_node **second_pass()
{
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

            for (int frame = start; frame < end; ++frame)
            {
                if (frame > num_frames)
                    break;
                struct vary_node *temp = malloc(sizeof(struct vary_node));
                strncpy(temp->name, name, 128);
                temp->value = val_i;
                temp->next = NULL;
                struct vary_node *it = knobs[frame];
                if (it)
                {
                    while (it->next) it = it->next;
                    it->next = temp;
                }
                else
                    knobs[frame] = temp;
                val_i += dv;
            }
        }
    }
    return knobs;
}

void my_main()
{
    color ambient;
    struct light *lights[MAX_LIGHTS] = {0};
    int num_lights = 0;
    double view[3];

    /* default values */
    SHADING_MODE = FLAT;

    NUM_POLY = 250;
    SHINYNESS = 1.6;
    ambient.r = 200;
    ambient.g = 200;
    ambient.b = 200;

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

    struct vary_node **knobs;
    /* struct vary_node *vn; */
    first_pass();
    knobs = second_pass();
    char frame_name[128];

    int cur_frame = 0;

    /* initialize image */
    Image *s = init_image();
    zbuffer *zb = init_zbuffer();
    do
    {
        sprintf(frame_name, "anim/%s_%03d.png", name, cur_frame);

        /* set knob values in symbol table */
        if (num_frames > 0)
        {
            struct vary_node *vn;
            vn = knobs[cur_frame];
            while (vn)
            {
                SYMTAB *s = lookup_symbol(vn->name);
                set_value(s, vn->value);
                vn = vn->next;
            }
        }

        /* initialize matrices for storing points */
        struct matrix *edges, *polygons;
        edges = new_matrix(4, 0);
        polygons = new_matrix(4, 0);

        /* initialize systems for storing coordinate systems */
        struct stack *systems = new_stack();
        struct matrix *transform;

        for (int i = 0; i < lastop; ++i)
        {
            struct constants *constants = reflect;
            struct matrix *cs = peek(systems);
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
                    
                    if (SHADING_MODE == FLAT)
                        draw_polygons(polygons, *s, *zb,
                                      view, lights, ambient, *constants);
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

                    if (SHADING_MODE == FLAT)
                        draw_polygons(polygons, *s, *zb,
                                      view, lights, ambient, *constants);
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

                    if (SHADING_MODE == FLAT)
                        draw_polygons(polygons, *s, *zb,
                                      view, lights, ambient, *constants);
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
                    if (op[i].op.scale.p)
                    {
                        SYMTAB *s = lookup_symbol(op[i].op.scale.p->name);
                        x *= s->s.value;
                        y *= s->s.value;
                        z *= s->s.value;
                    }
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
                    if (op[i].op.move.p)
                    {
                        SYMTAB *s = lookup_symbol(op[i].op.move.p->name);
                        x *= s->s.value;
                        y *= s->s.value;
                        z *= s->s.value;
                    }
                    transform = make_translate(x, y, z);
                    matrix_mult(cs, transform);
                    copy_matrix(transform, cs);
                    free_matrix(transform);
                    break;
                }
                case ROTATE:
                {
                    double theta = op[i].op.rotate.degrees;
                    if (op[i].op.rotate.p)
                    {
                        SYMTAB *s = lookup_symbol(op[i].op.rotate.p->name);
                        theta *= s->s.value;
                    }

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
                 * systems operations
                 */
                case PUSH:
                {
                    transform = new_matrix(4, 4);
                    copy_matrix(peek(systems), transform);
                    push(systems, transform);
                    break;
                }
                case POP:
                {
                    pop(systems);
                    break;
                }
                case SAVE_COORDS:
                {
                    copy_matrix(peek(systems), op[i].op.save_coordinate_system.p->s.m);
                    break;
                }

                /*
                 * lighting
                 */
                case AMBIENT:
                {
                    ambient.r = op[i].op.ambient.c[0];
                    ambient.g = op[i].op.ambient.c[1];
                    ambient.b = op[i].op.ambient.c[2];
                    break;
                }

                case LIGHT:
                {
                    struct light *l = malloc(sizeof(struct light));
                    l->l[0] = op[i].op.light.l[0];
                    l->l[1] = op[i].op.light.l[1];
                    l->l[2] = op[i].op.light.l[2];
                    l->c[0] = op[i].op.light.c[0];
                    l->c[1] = op[i].op.light.c[1];
                    l->c[2] = op[i].op.light.c[2];
                    lights[num_lights++] = l;
                    break;
                }

                /*
                 * misc
                 */
                case SHADING:
                {
                    char *s = op[i].op.shading.p->name;
                    if (strcmp("flat", s) == 0)
                    {
                        SHADING_MODE = FLAT;
                    }
                    else if (strcmp("raytrace", s) == 0)
                    {
                        SHADING_MODE = RAYTRACE;
                    }
                    break;
                }
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

        clear_zbuffer(*zb);
        clear_image(*s);
        free_stack(systems);
        free_matrix(edges);
        free_matrix(polygons);
    }
    while (++cur_frame < num_frames);

    if (num_frames > 0)
        make_animation(name);

    free(s);
    free(zb);
}
