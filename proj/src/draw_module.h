#ifndef _DRAW_MODULE_H_
#define _DRAW_MODULE_H_

//#define DRAW_SCREEN_H 880
//#define DRAW_SCREEN_V 650

#define DRAW_SCREEN_H 880 //200
#define DRAW_SCREEN_V 650 //200

#define DRAW_SCREENX_UL_CORNER 122 // 500 - 122
#define DRAW_SCREENY_UL_CORNER 30 // 200 - 30

typedef struct
{
  short* addr;
  unsigned int h_dim, v_dim;
  unsigned int x_ul_corner, y_ul_corner;
} draw_screen_area;

void drawMode(int irq_set_mouse, int irq_set_timer, short* draw_scr);

void mouseClickDrawEvent();

void keyboardDrawEvent();

void brush_handler();

#endif
