#include <stdio.h>
#include <stdlib.h>

#include "image.h"
#include "imageio.h"
#include "draw.h"

void plot(int x, int y, Image s, color c)
{
    /* NOTE: (0, 0) is the bottom left corner */
    y = YRES - 1 - y;
    if (x >= 0 && x < XRES && y >= 0 && y < YRES)
    {
        s[x][y] = c;
    }
}

void draw_line(int x0, int y0, int x1, int y1, Image s, color c)
{
    int x, y;
    int dx, dy;
    /* D = Ax + By + C, where A = dy, B = -dx, and C is irrelevant */
    int A, B, D;
    if (x1 < x0)
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;

        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    x = x0;
    y = y0;
    dx = x1 - x0;
    dy = y1 - y0;

    /* Note everything is scaled by 2 to avoid the pesky floating point number 1/2 */
    A = dy * 2;
    B = -dx * 2;
    if (abs(dx) >= abs(dy)) // m <= 1
    {
        D = 2 * dy - dx;
        if (dy >= 0)
        {
            while (x <= x1)
            {
                plot(x, y, s, c);
                if (D > 0) // next midpoint is below the line
                {
                    y++;
                    D += B;
                }
                x++;
                D += A;
            }
        }
        else
        {
            while (x <= x1)
            {
                plot(x, y, s, c);
                if (D < 0) // next midpoint is above the line
                {
                    y--;
                    D -= B;
                }
                x++;
                D += A;
            }
        }
    }
    else
    {
        D = dy - 2 * dx;
        dx *= 2;
        dy *= 2;
        if (dy >= 0)
        {
            while (y <= y1)
            {
                plot(x, y, s, c);
                if (D < 0) // next midpoint is above the line
                {
                    x++;
                    D += A;
                }
                y++;
                D += B;
            }
        }
        else
        {
            while (y >= y1)
            {
                plot(x, y, s, c);
                if (D > 0) // next midpoint is below the line
                {
                    x++;
                    D += A;
                }
                y--;
                D -= B;
            }
        }
    }
}