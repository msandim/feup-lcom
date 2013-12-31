#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

unsigned int vg_get_h_res();
unsigned int vg_get_v_res();
unsigned int vg_get_bits_per_pixel();

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
 * @param color color to fill the screen with
 * @param buf buffer where to do the action
 * @return 0 on success, non-zero upon failure
 */

int vg_fill(unsigned long color);
int vg_fill_buffer(unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

/**
 * @brief Sets input pixel with input color
 * 
 * Sets the color of the pixel at the specified position to the input value, 
 *  by writing to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param color color to set the pixel
 * @param buf buffer where to do the action
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
 * @param buf buffer where to do the action
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
long vg_get_pixel_buffer(unsigned long x, unsigned long y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

long vg_get_pixel(unsigned long x, unsigned long y);


/**
 * @brief Draw line segment with specifed end points and color
 * 
 * Draws a line segment with the specified end points and the input color, 
 *  by writing to VRAM
 * 
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)  
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)  
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)  
 * @param color color of the line segment to draw
 * @param buf buffer where to do the action
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_line_buffer(unsigned long xi, unsigned long yi,
    unsigned long xf, unsigned long yf, unsigned long color, unsigned short* buffer,
    unsigned long dim_h, unsigned long dim_v);
/**
 * @brief Draw object
 *
 * Draws a object
 */
void vg_draw_object(unsigned short* object, int w, int h, int x, int y, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_draw_circle_buffer(unsigned int x, unsigned int y, unsigned int radius, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_draw_rectangle_buffer(unsigned int x, unsigned int y, unsigned int x_dim, unsigned int y_dim, unsigned long color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_flood_fill_buffer(int x, int y, unsigned long replacement_color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_draw_brush_buffer(int xi, int yi, int xf, int yf, unsigned short color, unsigned int thickness, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_draw_text_buffer(int x, int y, char* text, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

void vg_draw_char_buffer(unsigned short* object, int w, int h, int x, int y, unsigned short color, unsigned short* buffer, unsigned long dim_h, unsigned long dim_v);

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
