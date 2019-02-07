#include "image.h"

#ifndef DISPLAY_H
#define DISPLAY_H

void clear_Image(Image s);
void save_image(Image s, char *file);
void write_image(Image s, FILE *f);
void display(Image s);

#endif
