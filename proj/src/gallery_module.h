#ifndef _GALLERY_MODULE_H_
#define _GALLERY_MODULE_H_

/**
 * @brief Returns SPRITE to be drawn
 *
 * Returns the SPRITE correspondent to the drawing beeing seen in the gallery
 *
 * @return drawing SPRITE
 */
SPRITE getFileDraw();

/**
 * @brief Returns total
 *
 * @return number of existing drawings
 */
int getTotal();

/**
 * @brief Returns file number
 *
 * @return number of drawing beeing seen in the gallery
 */
int getFileNumber();

/**
 * @brief Sets file number
 *
 * Sets file number to value passed in number argument
 */
void setNumber(int number);

/**
 * @brief Increments total
 *
 * Adds one to total number of files
 */
void incTotal ();

/**
 * @brief Loads Gallery
 *
 * Loads image used by the gallery from file
 *
 * @return 0 on success, 1 on error
 */
int galleryModeLoad();

/**
 * @brief Frees Gallery
 *
 * Frees memory used by image used by the gallery
 */
void galleryModeFree();

/**
 * @brief Ínitializes Gallery
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

#endif
