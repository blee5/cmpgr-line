#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "image.h"
#include "imageio.h"

Image *init_image()
{
    Image *s = malloc(sizeof (Image));
    clear_image(*s);
    return s;
}

zbuffer *init_zbuffer()
{
    zbuffer *zb = malloc(sizeof (zbuffer));
    clear_zbuffer(*zb);
    return zb;
}

void clear_image(Image s)
{
    int x, y;
    color c;
    c.r = DEFAULT_COLOR;
    c.g = DEFAULT_COLOR;
    c.b = DEFAULT_COLOR;
    for (y = 0; y < YRES; y++)
        for (x = 0; x < XRES; x++)
            s[x][y] = c;
}

void clear_zbuffer(zbuffer zb)
{
    int x, y;
    for (y = 0; y < YRES; y++)
        for (x = 0; x < XRES; x++)
            zb[x][y] = LONG_MIN;
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

void make_animation(char *name)
{
    int e, f;
    char name_arg[128];
    char gif_name[128];

    sprintf(name_arg, "anim/%s*", name);
    sprintf(gif_name, "%s.gif", name);
    printf("Making animation: %s\n", gif_name);
    f = fork();
    if (f == 0)
    {
        e = execlp("convert", "convert", "-delay", "1.7", name_arg, gif_name, NULL);
        printf("e: %d errno: %d: %s\n", e, errno, strerror(errno));
    }
    else
    {
        wait(NULL);
        printf("Animation created.\n");
    }
}

