#include "tracer.h"

void ray_trace(Image s, struct matrix *polygons, struct light **lights, double *view)
{
    for (int x = 0; x < XRES; ++x)
    {
        for (int y = 0; y < YRES; ++y)
        {
            double prim_ray[3];
            prim_ray[0] = (x + 0.5) - view[0];
            prim_ray[1] = (y + 0.5) - view[1];
            prim_ray[2] = view[2];
            double min_dist = 1e8;
        }
    }
}

