#ifndef _DRAW_MODULE_H_
#define _DRAW_MODULE_H_

#include "graphic_module.h"

#define DRAW_SCREENX_UL_CORNER 122
#define DRAW_SCREENY_UL_CORNER 30

#define COLOR_BARX_UL_CORNER 122
#define COLOR_BARY_UL_CORNER 700
#define COLOR_BAR_H_LENGTH 882
#define COLOR_BAR_V_LENGTH 62

#define MAX_COMMANDS_PER_UPDATE 7

// VARIABLES FOR DRAWING
#define MAX_THICKNESS 200
#define MIN_THICKNESS 1

typedef enum {st0, st1, st2, st3} tool_state;

unsigned short* getDrawScreen();

draw_screen_area getDrawScreenInfo();

BTN* getButtonArray();

SPRITE getColorBar();

unsigned short getColorSelected();


int drawModeLoad();

void drawModeFree();

void drawModeInit(int enable_serial_com);


void mouseDrawEvent();

int keyboardDrawEvent();

void checkPixelUpdate();

void blank_handler();

void reset_draw_screen();

void brush_handler();

void flood_fill_handler();

void color_picker_handler();

void circle_handler();

void rectangle_handler();

void rect_line_handler();

void selected_area_handler();

void date_draw_handler();

#endif
