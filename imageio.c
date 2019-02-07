#include <stdio.h>
#include <stdlib.h>

#include "image.h"
#include "imageio.h"

void clear_Image(Image s)
{
    int x, y;
    color c;
    c.r = DEFAULT_COLOR;
    c.g = DEFAULT_COLOR;
    c.b = DEFAULT_COLOR;
    for (y = 0; y < YRES; y++)
    {
        for (x = 0; x < XRES; x++)
        {
            s[x][y] = c;
        }
    }
}

/* Save image as a file */
void save_image(Image s, char *file)
{
    FILE *f;
    char line[256];
    sprintf(line, "convert - %s", file);

    f = popen(line, "w");
    write_image(s, f);
}

/* Display image */
void display(Image s)
{
    FILE *f;

    f = popen("display", "w");
    write_image(s, f);
}

/* Write image data to a file */
void write_image(Image s, FILE *f)
{
    int x, y;
    fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
    for (y = 0; y < YRES; y++)
    {
        for (x = 0; x < XRES; x++)
        {
            fprintf(f, "%d %d %d ", s[x][y].r, s[x][y].g, s[x][y].b);
        }
        fprintf(f, "\n");
    }
    pclose(f);
}
