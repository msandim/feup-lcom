#ifndef _DRAW_MODULE_H_
#define _DRAW_MODULE_H_

#define DRAW_SCREEN_H 400
#define DRAW_SCREEN_V 600

void drawMode(int irq_set_mouse, int irq_set_timer, short* draw_scr);

void mouseClickDrawAction();

#endif
