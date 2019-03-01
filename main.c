#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    Image *s = init_image();
    color c;
    c.r = 0; c.g = 0; c.b = 0;

    struct matrix *edges;
    struct matrix *transform;
    char *script;

    edges = new_matrix(4, 0);
    transform = ident(4);

    script = "stdin";
    if (argc == 2)
    {
        script = argv[1];
    }

    add_edge(edges, 0, 0, 0, 100, 0, 0);
    add_edge(edges, 100, 0, 0, 100, 100, 0);
    add_edge(edges, 100, 100, 0, 0, 100, 0);
    add_edge(edges, 0, 100, 0, 0, 0, 0);

    add_edge(edges, 0, 0, 0, 50, 50, 50);
    add_edge(edges, 0, 100, 0, 50, 50, 50);
    add_edge(edges, 100, 100, 0, 50, 50, 50);
    add_edge(edges, 100, 0, 0, 50, 50, 50);

    add_edge(edges, 0, 0, 0, 50, 50, -50);
    add_edge(edges, 0, 100, 0, 50, 50, -50);
    add_edge(edges, 100, 100, 0, 50, 50, -50);
    add_edge(edges, 100, 0, 0, 50, 50, -50);

    make_translate(transform, -200, -200, 0);
    make_rotX(transform, 20);
    matrix_mult(transform, edges);
    clear_transform(transform);

    make_translate(transform, -250, -250, -50);
    make_rotX(transform, 5);
    make_rotY(transform, 5);
    make_rotZ(transform, 15);
    make_scale(transform, 0.992, 0.992, 0.992);
    make_translate(transform, 250, 250, 50);

    /* parse_file(script, transform, edges, *s); */
    for (int i = 0; i < 10000; i++)
    {
        if (c.r + 1 >= 256)
            c.g = c.b = c.r = 255;
        else
        {
            c.r += 1;
            c.g += 1;
            c.b += 1;
        }
        draw_lines(edges, *s, c);
        matrix_mult(transform, edges);
    }
    display(*s);
    save_image(*s, "boo.png");
    free(s);
    free_matrix(edges);
    free_matrix(transform);
}
