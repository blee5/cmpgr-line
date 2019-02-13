#include "image.h"

#ifndef DISPLAY_H
#define DISPLAY_H

Image *init_image();
void clear_image(Image s);
void save_image(Image s, char *file);
void write_image(Image s, FILE *f);
void display(Image s);

#endif
