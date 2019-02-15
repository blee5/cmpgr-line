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
    add_column(n, 0.99, 0, 0, 0);
    add_column(n, 0, 0.90, 0, 0);
    add_column(n, 0, 0, 1, 0);
    add_column(n, 0, 0, 0, 1);

    add_edge(m, 300, 1500, 0, 100, 1800, 0);
    add_edge(m, 400, 800, 0, 200, 1000, 0);
    add_edge(m, 500, 1000, 0, 300, 1500, 0);
    add_edge(m, 600, 300, 0, 400, 1500, 0);
    draw_lines(m, *s, c);

    int i;
    for (i = 10000; i != 0; i--)
    {
        matrix_mult(n, m);
        draw_lines(m, *s, c);
    }
    display(*s);

    free_matrix(m);
    free_matrix(n);
    free(s);
    return 0;
}
