#ifndef _GALLERY_MODULE_H_
#define _GALLERY_MODULE_H_

/** @defgroup Gallery_Module Gallery Module
 * @{
 *
 * Module to work with gallery
 */

/**
 * @brief Returns the buffer with the file's drawing to be drawn
 *
 * Returns the buffer containing the drawing beeing seen in the gallery
 *
 * @return drawing SPRITE
 */
unsigned short* getFileDraw();

/**
 * @brief Returns total variable
 *
 * @return number of existing drawings
 */
int getTotal();

/**
 * @brief Returns file number
 *
 * @return number of the drawing beeing seen in the gallery
 */
int getFileNumber();

/**
 * @brief Sets file number
 *
 * Sets file number to value passed in number argument
 *
 * @param number of the drawing
 */
void setNumber(int number);

/**
 * @brief Increments total
 *
 * Adds one to total number of files
 */
void incTotal();

/**
 * @brief Loads Gallery Mode
 *
 * Loads space for the image showed in the gallery. Sets the total number of drawings
 *
 * @return 0 on success, 1 on error
 */
int galleryModeLoad();

/**
 * @brief Frees Gallery
 *
 * Frees memory used by image showed in the gallery.
 */
void galleryModeFree();

/**
 * @brief Initializes Gallery
 *
 * Initialize Gallery Mode, setting file number to 0 and checking if there are files
 */
void galleryModeInit();

/**
 * @brief Handles Gallery keyboard events
 *
 * Receive key strokes and handle them
 */
void keyboardGalleryEvent();

/**
 * @brief Handles Gallery mouse events
 *
 * @return 0 on success, 1 if user selected edit, -1 if user selected exit
 */
int mouseGalleryEvent();

/** @} */

#endif
