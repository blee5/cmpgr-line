#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "draw.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    Image *s = init_image();
    zbuffer *zb = init_zbuffer();

    struct matrix *edges, *polygons;
    char *script;

    edges = new_matrix(4, 0);
    polygons = new_matrix(4, 0);

    /* srand(time(NULL)); */

    script = "stdin";
    if (argc == 2)
    {
        script = argv[1];
    }

    parse_file(script, edges, polygons, *s, *zb);
    free(s);
}
