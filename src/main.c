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

    struct matrix *edges, *polygons;
    struct matrix *transform;
    char *script;

    edges = new_matrix(4, 0);
    polygons = new_matrix(4, 0);
    transform = new_matrix(4, 4);

    script = "stdin";
    if (argc == 2)
    {
        script = argv[1];
    }

    parse_file(script, transform, edges, polygons, *s);
    free(s);
}
