#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "imageio.h"
#include "draw.h"

int main() {

    Image s;
    color c;

    c.r = MAX_COLOR;
    c.g = MAX_COLOR;
    c.b = MAX_COLOR;

    clear_Image(s);

    /* Left side */
    draw_line(100, 175, 100, 200, s, c);
    draw_line(100, 300, 100, 325, s, c);
    draw_line(85, 300, 115, 300, s, c);
    draw_line(85, 300, 85, 200, s, c);
    draw_line(85, 200, 115, 200, s, c);
    draw_line(115, 200, 115, 300, s, c);

    /* Right side */
    draw_line(400, 200, 400, 175, s, c);
    draw_line(400, 300, 400, 325, s, c);
    draw_line(385, 300, 415, 300, s, c);
    draw_line(385, 300, 385, 200, s, c);
    draw_line(385, 200, 415, 200, s, c);
    draw_line(415, 200, 415, 300, s, c);

    /* Bottom side */
    draw_line(175, 100, 200, 100, s, c);
    draw_line(300, 100, 325, 100, s, c);
    draw_line(200, 115, 300, 115, s, c);
    draw_line(200, 115, 200, 85, s, c);
    draw_line(200, 85, 300, 85, s, c);
    draw_line(300, 115, 300, 85, s, c);

    /* Top side */
    draw_line(175, 400, 200, 400, s, c);
    draw_line(300, 400, 325, 400, s, c);
    draw_line(200, 415, 300, 415, s, c);
    draw_line(200, 415, 200, 385, s, c);
    draw_line(200, 385, 300, 385, s, c);
    draw_line(300, 415, 300, 385, s, c);

    /* Lower left corner */
    draw_line(75, 75, 75, 175, s, c);
    draw_line(75, 175, 125, 175, s, c);
    draw_line(75, 75, 175, 75, s, c);
    draw_line(175, 125, 175, 75, s, c);
    draw_line(125, 175, 175, 125, s, c);

    /* Upper left corner */
    draw_line(75, 325, 75, 425, s, c);
    draw_line(75, 325, 125, 325, s, c);
    draw_line(75, 425, 175, 425, s, c);
    draw_line(175, 425, 175, 375, s, c);
    draw_line(125, 325, 175, 375, s, c);

    /* Upper right corner */
    draw_line(325, 425, 425, 425, s, c);
    draw_line(325, 425, 325, 375, s, c);
    draw_line(425, 425, 425, 325, s, c);
    draw_line(375, 325, 425, 325, s, c);
    draw_line(325, 375, 375, 325, s, c);

    /* Lower right corner */
    draw_line(325, 125, 325, 75, s, c);
    draw_line(425, 75, 425, 175, s, c);
    draw_line(325, 75, 425, 75, s, c);
    draw_line(375, 175, 425, 175, s, c);
    draw_line(325, 125, 375, 175, s, c);

    /* Circle */
    draw_line(250, 325, 275, 320, s, c);
    draw_line(275, 320, 300, 300, s, c);
    draw_line(300, 300, 320, 275, s, c);
    draw_line(320, 275, 325, 250, s, c);
    draw_line(325, 250, 320, 225, s, c);
    draw_line(320, 225, 300, 200, s, c);
    draw_line(300, 200, 275, 180, s, c);
    draw_line(275, 180, 250, 175, s, c);
    draw_line(250, 325, 225, 320, s, c);
    draw_line(225, 320, 200, 300, s, c);
    draw_line(200, 300, 180, 275, s, c);
    draw_line(180, 275, 175, 250, s, c);
    draw_line(175, 250, 180, 225, s, c);
    draw_line(180, 225, 200, 200, s, c);
    draw_line(200, 200, 225, 180, s, c);
    draw_line(225, 180, 250, 175, s, c);

    /* Pink */
    c.r = 255;
    c.g = 117;
    c.b = 229;

    /* Heart */
    draw_line(250, 270, 240, 278, s, c);
    draw_line(240, 278, 225, 283, s, c);
    draw_line(225, 283, 210, 270, s, c);
    draw_line(210, 270, 205, 265, s, c);
    draw_line(205, 265, 200, 250, s, c);
    draw_line(200, 250, 205, 235, s, c);
    draw_line(205, 235, 250, 200, s, c);
    draw_line(250, 270, 260, 278, s, c);
    draw_line(260, 278, 275, 283, s, c);
    draw_line(275, 283, 290, 270, s, c);
    draw_line(290, 270, 295, 265, s, c);
    draw_line(295, 265, 300, 250, s, c);
    draw_line(300, 250, 295, 235, s, c);
    draw_line(295, 235, 250, 200, s, c);

    draw_line(115, 250, 175, 250, s, c);
    draw_line(325, 250, 385, 250, s, c);
    draw_line(250, 325, 250, 385, s, c);
    draw_line(250, 115, 250, 175, s, c);

    display(s);
    save_image(s, "cube.png");
}
