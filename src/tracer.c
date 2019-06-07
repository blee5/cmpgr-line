#include "tracer.h"

void ray_trace(Image *s, struct matrix *polygons)
{
    for (int x = 0; x < XRES; ++x)
    {
        for (int y = 0; y < YRES; ++y)
        {
            s[x][y]->r = 30; 
            s[x][y]->g = 30; 
            s[x][y]->b = 30; 
        }
    }
}
