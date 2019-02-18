#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "imageio.h"
#include "draw.h"
#include "matrix.h"

void matrix_test()
{
    struct matrix *a = new_matrix(4, 4);
    struct matrix *b = new_matrix(4, 2);
    printf("Adding to a matrix:\n");
    add_column(a, 1, 2, 3, 4);
    add_column(a, 5, 6, 7, 8);
    add_column(a, 9, 10, 11, 12);
    add_column(a, 13, 14, 15, 16);
    print_matrix(a);
    printf("Matrix multiplication:\n");
    printf("Multiplying above matrix with:\n");
    add_edge(b, 2, 2, 2, 0, 4, 0);
    print_matrix(b);
    matrix_mult(a, b);
    printf("Result:\n");
    print_matrix(b);
    printf("Turning a matrix into an identity matrix:\n");
    ident(a);
    print_matrix(a);

    free_matrix(a);
    free_matrix(b);
}

int main()
{
    Image *s = init_image();
    color c; c.r = 30; c.b = 100; c.g = 230;
    int i;
    struct matrix *points = new_matrix(4, 0);
    struct matrix *n = new_matrix(4, 4);
    matrix_test();
    add_column(n, 0.999, 0, 0, 0);
    add_column(n, 0, 0.97, 0, 0);
    add_column(n, 0, 0, 1, 0);
    add_column(n, 0, 0, 0, 1);

    add_edge(points, 0, 2000, 0, 100, 1800, 0);
    add_edge(points, 100, 1800, 0, 200, 1400, 0);
    add_edge(points, 200, 1400, 0, 300, 800, 0);
    add_edge(points, 300, 800, 0, 400, 1400, 0);
    add_edge(points, 400, 1400, 0, 500, 1800, 0);
    add_edge(points, 500, 1800, 0, 1000, 2000, 0);
    draw_lines(points, *s, c);

    for (i = 1000; i != 0; i--)
    {
        c.g = (c.g + i / 4) % 256;
        c.b = (c.b + i / 2) % 256;
        matrix_mult(n, points);
        draw_lines(points, *s, c);
    }
    display(*s);
    save_image(*s, "out.png");

    free(s);
    free_matrix(points);
    free_matrix(n);
    return 0;
}
