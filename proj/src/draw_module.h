#ifndef _DRAW_MODULE_H_
#define _DRAW_MODULE_H_

#include "graphic_module.h"

#define DRAW_SCREENX_UL_CORNER 122
#define DRAW_SCREENY_UL_CORNER 30

#define COLOR_BARX_UL_CORNER 122
#define COLOR_BARY_UL_CORNER 700
#define COLOR_BAR_H_LENGTH 882
#define COLOR_BAR_V_LENGTH 62

#define MAX_COMMANDS_PER_UPDATE 2000

// VARIABLES FOR DRAWING
#define MAX_THICKNESS 200
#define MIN_THICKNESS 1

#define F_MAKECODE 0x21
#define B_MAKECODE 0x30
#define K_MAKECODE 0x25
#define I_MAKECODE 0x17
#define C_MAKECODE 0x2E
#define R_MAKECODE 0x13
#define L_MAKECODE 0x26
#define V_MAKECODE 0x2F
#define M_MAKECODE 0x32
#define G_MAKECODE 0x22
#define T_MAKECODE 0x14
#define S_MAKECODE 0x1F
#define ESC_MAKECODE 0x01

#define PLUS1_MAKECODE 0x4E
#define PLUS2_MAKECODE 0x1A
#define MINUS1_MAKECODE 0x4A
#define MINUS2_MAKECODE 0x35



typedef enum {st0, st1, st2, st3} tool_state;

unsigned short* getDrawScreen();

draw_screen_area getDrawScreenInfo();

BTN* getButtonArray();

SPRITE getColorBar();

unsigned short getColorSelected();


int drawModeLoad();

void drawModeFree();

void drawModeInit(int enable_serial_com, int file_name);


void mouseDrawEvent();

int keyboardDrawEvent();

void checkCommandUpdate();


void blank_handler();

void brush_handler();

void flood_fill_handler();

void color_picker_handler();

void circle_handler();

void rectangle_handler();

void rect_line_handler();

void selected_area_handler();

void mirror_effect_handler();

void magic_bucket_handler();

void date_draw_handler();

void save_handler();

#endif
