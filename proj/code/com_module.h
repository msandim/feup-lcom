#ifndef _COM_MODULE_H_
#define _COM_MODULE_H_

/** @defgroup com_module Communication Module
 * @{
 *
 * Higher level module that handles with the UART and receives/transfers drawing commands
 */

#define DEFAULT_BITRATE 19200 /**< @brief Default bitrate */

#include "time_module.h"

/**
 * @brief Sets the draw_screen to the same as in draw_module to draw commands
 * @param draw Pointer to the draw_screen in draw_module
 */
void setComDrawScreen(unsigned short* draw);

/**
 * @brief Handles command receiving
 *
 * This function is responsible for checking if new data is coming from another computer
 * and process it. It's called frequently in runDevices() and handles data coming from another computer,
 * analyzing and separating drawing commands, and calling their handlers.
 *
 *
 * @return 1 if there is not data available or an error occured, 0 if not
 */
int command_handler();

/**
 * @brief Circle command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for circles
 *
 */
void commandCircleHandler();

/**
 * @brief Rectangle command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for rectangle
 *
 */
void commandRectangleHandler();

/**
 * @brief Line (brush) command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for lines
 *
 */
void commandLineHandler();

/**
 * @brief Flood Fill command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for the flood fill action
 *
 */
void commandFloodFillHandler();

/**
 * @brief Date Draw command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for date draws
 *
 */
void commandDateDrawHandler();

/**
 * @brief Blank command handler
 *
 * Responsible for "build" a command out of chars received and making the drawing action for the blank action
 *
 */
void commandBlankHandler();

/**
 * @brief Circle Command Sender
 *
 * Responsible for sending a command to draw a circle in another computer
 * @param x x position of the circle center in the draw screen
 * @param y y position of the circle center in the draw screen
 * @param radius radius of the circle
 * @param color color of the circle
 * @return 0 on success, non-zero otherwise
 */
int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned long color);

/**
 * @brief Rectangle Command Sender
 *
 * Responsible for sending a command to draw a rectangle in another computer
 * @param x x position of the left-upper corner of the rectangle in the draw screen
 * @param y y position of the left-upper corner of the rectangle in the draw screen
 * @param dim_h horizontal dimension of the rectangle
 * @param dim_v horizontal dimension of the rectangle
 * @param color color of the rectangle
 * @return 0 on success, non-zero otherwise
 */
int sendCommandRectangle(unsigned int x, unsigned int y, unsigned int dim_h, unsigned int dim_v, unsigned long color);

/**
 * @brief Line Command Sender
 *
 * Responsible for sending a command to draw a line in another computer
 * @param xi x position of the initial point in the draw screen
 * @param yi y position of the initial point in the draw screen
 * @param xf x position of the final point in the draw screen
 * @param yf y position of the final point in the draw screen
 * @param radius thickness of the line
 * @param color color of the line
 * @return 0 on success, non-zero otherwise
 */
int sendCommandLine(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf, unsigned int radius, unsigned long color);

/**
 * @brief Flood-Fill Command Sender
 *
 * Responsible for sending a command to start a flood-fill effect in another computer
 * @param x x position of the initial point in the draw screen
 * @param y y position of the initial point in the draw screen
 * @param color color to use in the flood-fill effect
 * @return 0 on success, non-zero otherwise
 */
int sendCommandFloodFill(unsigned int x, unsigned int y, unsigned long color);

/**
 * @brief Date Draw Command Sender
 *
 * Responsible for sending a command to draw the date in another computer
 * @param x x position of the initial point in the draw screen
 * @param y y position of the initial point in the draw screen
 * @param date information regarding the date to draw
 * @param color color to use in the date draw
 * @return 0 on success, non-zero otherwise
 */
int sendCommandDateDraw(unsigned int x, unsigned int y, Date_info date, unsigned long color);

/**
 * @brief Blank Command Sender
 *
 * Responsible for sending a command to clean the draw screen
 * @return 0 on success, non-zero otherwise
 */
int sendCommandBlank();

/**
 * @brief Mirror effect Command Sender
 *
 * Responsible for sending a command to make the mirror effect in the draw screen
 * @return 0 on success, non-zero otherwise
 */
int sendCommandMirrorEffect();

/**
 * @brief Magic Bucket effect Command Sender
 *
 * Responsible for sending a command to make the Magic Bucket effect in the draw screen
 * @return 0 on success, non-zero otherwise
 */
int sendCommandMagicBucketEffect();

/**
 * @brief Initiates Serial Port Communication with FIFOs
 *
 * @return 0 on success, non-zero otherwise
 */
int initSerialPort();

/**
 * @brief Shuts down Serial Port Communication with FIFOs
 *
 * @return 0 on success, non-zero otherwise
 */
int shutSerialPort();

/** @} */

#endif
