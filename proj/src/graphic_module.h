#ifndef _GRAPHIC_MODULE_H_
#define _GRAPHIC_MODULE_H_

void screenInit();

void screenExit();

int set_drawMode(unsigned int mouse_x, unsigned int mouse_y);

int draw_mouse(unsigned int mouse_x, unsigned int mouse_y);

#endif
