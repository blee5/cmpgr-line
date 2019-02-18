#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "imageio.h"
#include "draw.h"
#include "matrix.h"

int main()
{
    Image *s = init_image();
    color c;
    c.r = 30; c.b = 100; c.g = 230;

    struct matrix *m = new_matrix(4, 0);
    struct matrix *n = new_matrix(4, 4); 
    add_column(n, 0.999, 0, 0, 0);
    add_column(n, 0, 0.97, 0, 0);
    add_column(n, 0, 0, 1, 0);
    add_column(n, 0, 0, 0, 1);

    add_edge(m, 0, 2000, 0, 100, 1800, 0);
    add_edge(m, 100, 1800, 0, 200, 1400, 0);
    add_edge(m, 200, 1400, 0, 300, 800, 0);
    add_edge(m, 300, 800, 0, 400, 1400, 0);
    add_edge(m, 400, 1400, 0, 500, 1800, 0);
    add_edge(m, 500, 1800, 0, 1000, 2000, 0);
    print_matrix(m);
    draw_lines(m, *s, c);

    int i;
    for (i = 1000; i != 0; i--)
    {
        c.g = (c.g + i / 4) % 256;
        c.b = (c.b + i / 2) % 256;
        matrix_mult(n, m);
        draw_lines(m, *s, c);
    }
    display(*s);

    free_matrix(m);
    free_matrix(n);
    free(s);
    return 0;
}
