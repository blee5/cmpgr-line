#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "gmath.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    /* lighting values */
    color ambient;
    double light[2][3];
    double view[3];
    double a_reflect[3];
    double d_reflect[3];
    double s_reflect[3];

    ambient.r = 200;
    ambient.g = 200;
    ambient.b = 200;

    light[LOCATION][0] = -1;
    light[LOCATION][1] = 1;
    light[LOCATION][2] = 0;
    light[COLOR][RED] = 200;
    light[COLOR][BLUE] = 120;
    light[COLOR][GREEN] = 200;

    view[0] = 0; view[1] = 0; view[2] = 1;

    a_reflect[RED] = 0.2;
    a_reflect[BLUE] = 0.2;
    a_reflect[GREEN] = 0.2;

    d_reflect[RED] = 0.8;
    d_reflect[BLUE] = 0.8;
    d_reflect[GREEN] = 0;

    s_reflect[RED] = 1;
    s_reflect[BLUE] = 1;
    s_reflect[GREEN] = 1;
    Image *s = init_image();
    zbuffer *zb = init_zbuffer();

    struct matrix *edges, *polygons;
    char *script;

    edges = new_matrix(4, 0);
    polygons = new_matrix(4, 0);

    script = "stdin";
    if (argc == 2)
    {
        script = argv[1];
    }

    parse_file(script, edges, polygons, *s, *zb,
               view, ambient, light, a_reflect, s_reflect, d_reflect);
    free(s);

    return EXIT_SUCCESS;
}
