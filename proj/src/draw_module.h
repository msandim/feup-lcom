#ifndef _DRAW_MODULE_H_
#define _DRAW_MODULE_H_

#include "graphic_module.h"

#define DRAW_SCREEN_H 880
#define DRAW_SCREEN_V 650

#define DRAW_SCREENX_UL_CORNER 122
#define DRAW_SCREENY_UL_CORNER 30

#define COLOR_BARX_UL_CORNER 122
#define COLOR_BARY_UL_CORNER 700
#define COLOR_BAR_H_LENGTH 882
#define COLOR_BAR_V_LENGTH 62

// VARIABLES FOR DRAWING
#define MAX_THICKNESS 200
#define MIN_THICKNESS 1

void drawMode(int irq_set_mouse, int irq_set_kbd, int irq_set_timer, int irq_set_rtc, unsigned short* draw_scr,
    BTN* btn_array, SPRITE clr_bar);

void mouseClickDrawEvent();

void keyboardDrawEvent();

void blank_handler();

void brush_handler();

void flood_fill_handler();

void color_picker_handler();

void circle_handler();

void rectangle_handler();

void rect_line_handler();

void selected_area_handler();

void date_draw_handler();

#endif
