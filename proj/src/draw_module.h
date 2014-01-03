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


//State of the drawing tools
typedef enum {st0, st1, st2, st3} tool_state;

/**
 * @brief Returns the screen
 *
 * Returns the pixel array
 *
 * @return the pixel array in the screen
 */
 unsigned short* getDrawScreen();

/**
 * @brief Returns default_area
 *
 * Returns information about the default draw area to be used to draw the screen
 *
 * @return information about default draw area
 */
 draw_screen_area getDrawScreenInfo();

/**
 * @brief Returns button array
 *
 * Returns the array of buttons in order for them to be drawn
 *
 * @return array of buttons
 */
 BTN* getButtonArray();

/**
 * @brief Returns color bar SPRITE
 *
 * Returns the SPRITE representing the color bar in order for it to be drawn
 *
 * @return color bar SPRITE
 */
 SPRITE getColorBar();

/**
 * @brief Returns selected color
 *
 * Returns the selected color
 *
 * @return selected color
 */
 unsigned short getColorSelected();

/**
 * @brief Loads drawMode
 *
 * Loads all the information needed for the drawMode to work: draw_screen, buttons and color bar
 *
 * @return 0 on success, 1 if there was an error
 */
 int drawModeLoad();

/**
 * @brief Frees memory used by drawMode
 *
 * Frees memory used by the color bar, buttons and draw_screen
 */
 void drawModeFree();

/**
 * @brief Initialize drawMode
 *
 * Sets the initial values:
 * 	tool selected to 0 (fill)
 *	color selected to 0x2104 (black)
 *	thickness to 5
 *	tool state to st0
 *	buttons to off
 *
 *	If the user has chosen a file from the gallery loads the file, otherwise prints a welcome message
 *	
 *	If the user has chosen to use the Multi mode, enables serial port
 */

 void drawModeInit(int enable_serial_com, int file_name);


/**
 * @brief Handle mouse
 *
 * If the user pressed in the screen, activate selected tool
 *
 *	If the user pressed in the color bar, update selected color
 */
 void mouseDrawEvent();

/**
 * @brief Handle keyboard
 *
 * Activates tool based on received key stroke
 *
 * @return 0 on success, -1 if user pressed ESC to quit
 */
 int keyboardDrawEvent();

/**
 * @brief Checks updates on commands
 *
 * Loops until there are no more commands
 */
 void checkCommandUpdate();

/**
 * @brief Blank Handler
 *
 * Fills the screen with white color
 */
 void blank_handler();

/**
 * @brief Brush Handler
 *
 * While the button is pressed, draw circles with selected thickness next to each other
 */
 void brush_handler();

/**
 * @brief Flood Fill Handler
 *
 * Applies the Flood Fill at the location of the mouse click
 */
 void flood_fill_handler();

/**
 * @brief Color Picker Handler
 *
 * Sets current color to the one at the mouse location at the time of the click
 */
 void color_picker_handler();

/**
 * @brief Circle Handler
 *
 * Draws one circle receiving two mouse clicks, one at the center, and another at the extremity
 */
 void circle_handler();

/**
 * @brief Rectangle Handler
 *
 * Draws one rectangle receiving two mouse clicks at opposite corners
 */
 void rectangle_handler();

/**
 * @brief Line Handler
 *
 * Draws one line receiving two mouse clicks, one at each end point
 */
 void rect_line_handler();

/**
 * @brief Selected Area Handler
 *
 * Sets the draw area by defining a rectangle with two clicks at opposite corners
 */
 void selected_area_handler();

/**
 * @brief Mirror Effect Handler
 *
 * Applies the mirror effect
 */
 void mirror_effect_handler();

/**
 * @brief Magic Bucket Effect Handler
 *
 * Applies the magic bucket effect
 */
 void magic_bucket_handler();

/**
 * @brief Date Draw Handler
 *
 * Stamps actual time at mouse click position in the following format
 * HH - MM - SS DD - MM -YY
 */
 void date_draw_handler();

/**
 * @brief Save Handler
 *
 * Saves drawing for future use, copying buffer to file
 */
 void save_handler();

#endif
