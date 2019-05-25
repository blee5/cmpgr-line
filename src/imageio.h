#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>

#include "image.h"

Image *init_image();
zbuffer *init_zbuffer();
void clear_image(Image s);
void clear_zbuffer(zbuffer zb);
void save_image(Image s, char *file);
void write_image(Image s, FILE *f);
void display(Image s);
void make_animation(char *name);

#endif
