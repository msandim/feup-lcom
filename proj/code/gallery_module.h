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
 *
 * @return the buffer containing the drawing beeing seen in the gallery
 */
unsigned short* getFileDraw();

/**
 * @brief Returns total variable
 *
 * @return number of existing drawings
 */
int getTotalNumberDrawings();

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
void setFileNumber(int number);

/**
 * @brief Increments total number of drawings variable
 *
 * Adds one to total number of files (drawings)
 */
void incTotalNumberDrawings();

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
 * Receives keyboard events and handles them in the context of the gallery
 */
void keyboardGalleryEvent();

/**
 * @brief Handles Gallery mouse events
 *
 * Receives mouse events and handles them in the context of the gallery
 *
 * @return 0 on success, 1 if user selected edit, -1 if user selected exit
 */
int mouseGalleryEvent();

/** @} */

#endif
