#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/** @defgroup video_gr Video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Returns the static h_res variable
 * @return unsigned int value of h_res
 */
unsigned int vg_get_h_res();

/**
 * @brief Returns the static v_res variable
 * @return unsigned int value of v_res
 */
unsigned int vg_get_v_res();

/**
 * @brief Returns the static bits_per_pixel variable
 * @return unsigned int value of bits_per_pixel
 */
unsigned int vg_get_bits_per_pixel();

/**
 * @brief Copies a buffer (with the same size as video_mem) to video_mem using memcpy
 * @return pointer to the buffer to copy
 */
int vg_copy_buffer(unsigned short* buffer);

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void * vg_init(unsigned short mode);

/**
 * @brief Fills the screen with the input color
 * 
 * @param color color to fill the screen (video_mem) with
 * @return 0 on success, non-zero upon failure
 */
int vg_fill(unsigned long color);

/**
 * @brief Fills the buffer "buffer" with the input color
 *
 * @param color color to fill the buffer with
 * @param buffer pointer to the buffer
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 on success, non-zero upon failure
 */
int vg_fill_buffer(unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Sets input pixel with input color on the input buffer
 * 
 * Sets the color of the pixel at the specified position to the input value, 
 *  by writing to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param color color to set the pixel
 * @param buffer pointer to the buffer
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 on success, non-zero otherwise
 */
int vg_set_pixel_buffer(unsigned long x, unsigned long y, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Returns the color of the input pixel
 * 
 * Returns the color of the pixel at the specified position, 
 *  by reading to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
long vg_get_pixel(unsigned long x, unsigned long y);

/**
 * @brief Returns the color of the input pixel on the "buffer"
 *
 *
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
long vg_get_pixel_buffer(unsigned long x, unsigned long y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);


/**
 * @brief Draw line segment with specifed end points and color on the "buffer"
 * 
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)  
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)  
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)  
 * @param color color of the line segment to draw
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_line_buffer(unsigned long xi, unsigned long yi,
    unsigned long xf, unsigned long yf, unsigned long color, unsigned short* buffer,
    unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Draws an object on the "buffer"
 *
 * Draws an object on a color buffer passed as parameter.
 * The buffer is drawed in reverse, since it's loaded from a BMP
 *
 * @param object buffer that has the object's colors
 * @param w width (dimension in x) of the object
 * @param h height (dimension in y) of the object
 * @param x horizontal coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param y vertical coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_object_buffer(unsigned short* object, unsigned long w, unsigned long h, unsigned long x, unsigned long y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Draws a rectangle on the "buffer"
 *
 * @param x horizontal coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param y vertical coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param w x dimension (width of the rectangle)
 * @param h y dimension (height of the rectangle)
 * @param color color desired
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_rectangle_buffer(unsigned long x, unsigned long y, unsigned long w, unsigned long h, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Draws a circle on the "buffer"
 *
 * @param x horizontal coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param y vertical coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param color color desired
 * @param radius radius of the circle
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_circle_buffer(unsigned long x, unsigned long y, unsigned long radius, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Does the flood-fill effect on a buffer (bucket tool of paint)
 *
 * @param x horizontal coordinate of the initial point
 * @param y vertical coordinate of the initial point
 * @param replacement_color color using to apply the effect
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_flood_fill_buffer(unsigned long x, unsigned long y, unsigned long replacement_color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Draws on the "buffer" a line consisting of circles ("brush")
 *
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)
 * @param color value of the color to use
 * @param thickness radius of the circle to use in the painting
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_brush_buffer(unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned short color, unsigned long thickness, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Does the mirror effect on a buffer
 *
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_mirror_effect_buffer(unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Does the magic bucket effect on a buffer
 *
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_magic_bucket_effect_buffer(unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);
/**
 * @brief Draws a char on the "buffer"
 *
 * Draws a char on a color buffer passed as parameter.
 * The char as passed as parameter. It is printed in reverse, since it's loaded from a BMP
 * This function differs from the draw_object one, because it prints all the pixels that aren't white
 * in the char image.
 *
 * @param object buffer that has the object's (char) colors
 * @param w x dimension (width of the object)
 * @param h y dimension (height of the object)
 * @param x horizontal coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param y vertical coordinate of the left-upper corner in "buffer" where to draw the object (starts at 0)
 * @param color desired to paint the char
 * @param buffer buffer where to do the action
 * @param dim_h horizontal dimension of the buffer
 * @param dim_v vertical dimension of the buffer
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_char_buffer(unsigned short* object, int w, int h, int x, int y, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/** @} end of video_gr */

void vg_fill_ra(unsigned int x, unsigned int y,
    unsigned short color,
    unsigned short* buffer, unsigned int w, unsigned int h);

void _vg_fill(unsigned int x, unsigned int y, unsigned char dir);

#endif /* __VIDEO_GR_H */
