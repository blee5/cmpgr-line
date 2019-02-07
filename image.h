#ifndef ML6_H
#define ML6_H

#define XRES 500
#define YRES 500
#define MAX_COLOR 255
#define DEFAULT_COLOR 0

struct point_t
{
    int r;
    int g;
    int b;
} point_t;

typedef struct point_t color;

typedef struct point_t Image[XRES][YRES];

#endif
