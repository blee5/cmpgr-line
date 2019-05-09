#include "image.h"

#ifndef DISPLAY_H
#define DISPLAY_H

Image *init_image();
zbuffer *init_zbuffer();
void clear_image(Image s);
void clear_zbuffer(zbuffer zb);
void save_image(Image s, char *file);
void write_image(Image s, FILE *f);
void display(Image s);

#endif
