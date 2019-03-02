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

    /* make_translate(transform, -200, -200, 0); */
    /* make_rotX(transform, 20); */
    /* matrix_mult(transform, edges); */

    /* make_translate(transform, -250, -250, -50); */
    /* make_rotX(transform, 5); */
    /* make_rotY(transform, 5); */
    /* make_rotZ(transform, 15); */
    /* make_scale(transform, 0.992, 0.992, 0.992); */
    /* make_translate(transform, 250, 250, 50); */

    parse_file(script, transform, edges, *s);
    free(s);
    free_matrix(edges);
    free_matrix(transform);
}
