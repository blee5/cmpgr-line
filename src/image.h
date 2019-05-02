#ifndef ML6_H
#define ML6_H

#define XRES 500
#define YRES 500
#define MAX_COLOR 255
#define DEFAULT_COLOR 45

int NUM_POLY;
double SHINYNESS;

struct point_t
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} point_t;

typedef struct point_t color;

typedef struct point_t Image[XRES][YRES];
typedef double zbuffer[XRES][YRES];

#endif
