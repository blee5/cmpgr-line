#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "imageio.h"
#include "draw.h"
#include "matrix.h"

int main()
{
    struct matrix *m = new_matrix(5, 5);
    ident(m);
    print_matrix(m);
    return 0;
}
