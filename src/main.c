#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "gmath.h"
#include "image.h"
#include "imageio.h"
#include "matrix.h"
#include "parser.h"

int read_config(double light[2][3], double *view, color ambient,
                double *a_reflect, double *d_reflect, double *s_reflect)
{
    FILE *f = fopen(".config", "r");
    if (!f)
        return -1;
    char line[256];
    while (fgets(line, 255, f) != NULL)
    {
        line[strlen(line)-1] = '\0';
        if (strcmp(line, "view") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &view[0], &view[1], &view[2]);
        }
        else if (strcmp(line, "ambient") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%c %c %c", &ambient.r, &ambient.g, &ambient.b);
        }
        else if (strcmp(line, "a_reflect") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &a_reflect[0], &a_reflect[1], &a_reflect[2]);
        }
        else if (strcmp(line, "d_reflect") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &d_reflect[0], &d_reflect[1], &d_reflect[2]);
        }
        else if (strcmp(line, "s_reflect") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf", &s_reflect[0], &s_reflect[1], &s_reflect[2]);
        }
        else if (strcmp(line, "shinyness") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf", &SHINYNESS);
        }
        else if (strcmp(line, "num_polygons") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%d", &NUM_POLY);
        }
        else if (strcmp(line, "light") == 0)
        {
            fgets(line, 255, f);
            sscanf(line, "%lf %lf %lf %lf %lf %lf", &light[LOCATION][0],
                                                    &light[LOCATION][1],
                                                    &light[LOCATION][2],
                                                    &light[COLOR][0],
                                                    &light[COLOR][1],
                                                    &light[COLOR][2]);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    color ambient;
    double light[2][3];
    double view[3];
    double a_reflect[3];
    double d_reflect[3];
    double s_reflect[3];

    /* default lighting values */
    NUM_POLY = 30;
    SHINYNESS = 30;
    ambient.r = 200;
    ambient.g = 200;
    ambient.b = 200;

    light[LOCATION][0] = 1;
    light[LOCATION][1] = 1;
    light[LOCATION][2] = 1;
    light[COLOR][RED] = 200;
    light[COLOR][GREEN] = 200;
    light[COLOR][BLUE] = 200;

    view[0] = 0; view[1] = 0; view[2] = 1;

    a_reflect[RED] = 0.2;
    a_reflect[GREEN] = 0.2;
    a_reflect[BLUE] = 0.2;

    d_reflect[RED] = 1;
    d_reflect[GREEN] = 1;
    d_reflect[BLUE] = 1;

    s_reflect[RED] = 1;
    s_reflect[GREEN] = 1;
    s_reflect[BLUE] = 1;

    if (read_config(light, view, ambient, a_reflect, d_reflect, s_reflect) < 0)
    {
        printf("Could no read .config, using default values\n");
    }

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
