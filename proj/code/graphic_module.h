#ifndef _GRAPHIC_MODULE_H_
#define _GRAPHIC_MODULE_H_

/** @defgroup graphic_module Graphic Module
 * @{
 *
 * Graphic module that works with graphics and files
 */

#define DRAW_SCREEN_H 880 /**< @brief Horizontal size of the draw screen */
#define DRAW_SCREEN_V 650 /**< @brief Vertical size of the draw screen */

/** @name Header Struct*/
/**@{
 *
 * Header of BMP file
 */
typedef struct {
	unsigned short int type; /**< @brief Magic identifier */
	unsigned int size; /**< @brief File size in bytes          */
	unsigned short int reserved1; /**< @brief Reserved unsigned short */
	unsigned short int reserved2; /**< @brief Reserved unsigned short */
	unsigned int offset; /**< @brief Offset to image data, bytes */
} HEADER;
/** @}  */

/** @name Info Header Struct*/
/**@{
 *
 * Information Header of BMP file
 */
typedef struct {
   unsigned int size;               /**< @brief Header size in bytes      */
   int width;                       /**< @brief Height of image */
   int height;                      /**< @brief Width of image */
   unsigned short int planes;       /**< @brief Number of colour planes   */
   unsigned short int bits;         /**< @brief Bits per pixel            */
   unsigned int compression;        /**< @brief Compression type          */
   unsigned int imagesize;          /**< @brief Image size in bytes       */
   int xresolution;                 /**< @brief X resolution of the image */
   int yresolution;                 /**< @brief Y resolution of the image          */
   unsigned int ncolours;           /**< @brief Number of colours         */
   unsigned int importantcolours;   /**< @brief Important colours         */
} INFOHEADER;
/** @}  */

/** @name Sprite Struct*/
/**@{
 *
 * Structure that contains information of a graphic object
 */
typedef struct {
	unsigned short* pixels; /**< @brief Array with the pixels */
	int width; /**< @brief Width of the object */
	int height; /**< @brief Height of the object */
} SPRITE;
/** @}  */

/** @name Button Struct*/
/**@{
 *
 * Structure that contains information of a button
 */
typedef struct {
	SPRITE sprite_off; /**< @brief Sprite with the button image if it's not pressed */
	SPRITE sprite_on; /**< @brief Sprite with the button image if it's it's pressed */
	int press_state; /**< @brief Flag that tells if a button is pressed or not */
} BTN;
/** @}  */

/** @name Draw Screen Area Struct*/
/**@{
 *
 * Structure that contains information on the drawing portion of the screen
 */
typedef struct
{
  unsigned int h_dim; /**< @brief Horizontal dimension of the area */
  unsigned int v_dim; /**< @brief Vertical dimension of the area */
  unsigned int x_ul_corner; /**< @brief X coordinate of the upper left corner of the area */
  unsigned int y_ul_corner; /**< @brief Y coordinate of the upper left corner of the area */
} Draw_screen_area;
/** @}  */

/**
 * @brief Loads Graphics
 *
 * Loads all the graphics needed by the program:
 *  Characters for printing text
 *  Introduction Screen
 *  Menu Screen
 *  Gallery Screen
 *
 * Allocates memory for the drawing screen
 *
 * @return 0 on success, 1 on error
 */
int screenInit();

/**
 * @brief Frees Graphics
 *
 * Frees all the graphics needed by the program:
 *  Characters for printing text
 *  Introduction Screen
 *  Menu Screen
 *  Gallery Screen
 *  Drawing Screen
 */
void screenExit();

/**
 * @brief Copy drawing to VRAM
 *
 * Copies the current drawing to the VRAM
 */
void drawBufferInVRAM();

/**
 * @brief Draws Graphics in Draw Mode
 *
 * Prints all the graphics in Draw Mode:
 *  Current Drawing
 *  Tool Bar
 *  Color Bar
 *  Mouse
 *
 *  @param draw_screen array of colors of the draw screen
 *  @param draw_area default measures for the draw_area on the screen
 *  @param draw_area current measures for the draw_area on the screen
 *  @param btn_array array of buttons (tools)
 *  @param color_bar color bar sprite
 *  @param color_selected color selected by the user (used to paint the background)
 */
void set_graphicsDrawMode(unsigned short* draw_screen, Draw_screen_area draw_area, Draw_screen_area current_area, BTN* btn_array, SPRITE color_bar, unsigned short color_selected);

/**
 * @brief Draws Graphics in Intro
 *
 * Prints Image in Intro Screen
 */
void set_graphicsIntroMode();

/**
 * @brief Draws Graphics in Menu
 *
 * Prints Image in Menu Screen
 */
void set_graphicsMenuMode();

/**
 * @brief Draws Graphics in Gallery Mode
 *
 * Prints all the graphics in Gallery Mode:
 *  Current Drawing
 *  Buttons
 *  Mouse
 *
 *  @param file_drawing Buffer that contains the drawing to print
 *
 */
void set_graphicsGalleryMode(unsigned short* file_drawing);

/**
 * @brief Draws Area in Double Buffer
 *
 * Copies buffer of pixels to the Double Buffer
 *
 * @param buffer object to draw in the the Double Buffer
 * @param x_upperleft_corner x coordinate of the upper left corner of the area
 * @param y_upperleft_corner y coordinate of the upper left corner of the area
 * @param dim_h Horizontal dimension of the area
 * @param dim_v Vertical dimension of the area
 *
 * @return 0 on success, 1 if buffer does not fit Double Buffer
 */
int drawAreaInDoubleBuffer(unsigned short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v);

/**
 * @brief Checks equality of areas
 *
 * Compares horizontal and vertical dimensions of the areas, as well as positioning
 *
 * @param area1 First area to compare
 * @param area2 Second area to compare
 *
 * @return 0 if false, 1 if true
 */
int areasAreEqual(Draw_screen_area area1, Draw_screen_area area2);

/**
 * @brief Draws Mouse
 *
 * Draws cursor, taking in account the black areas of the screen, where the cursor becomes white
 *
 * @return 0
 */
void drawMouse();

/**
 * @brief Draws Tool Bar
 *
 * Takes an array of BTN and prints all the buttons, taking in account press states
 *
 * @param btnArray Array of buttons to print
 * @param buffer Buffer where to draw the btn array
 */
void drawToolBar(BTN* btnArray, unsigned short* buffer);

/**
 * @brief Draws Text (a-z),(0-9) and (-)
 *
 * Prints string character by character
 *
 * @param x x coordinate of the point where to start to draw
 * @param y y coordinate of the point where to start to draw
 * @param text String to draw
 * @param color color desired
 * @param buffer Buffer where to draw the text
 * @param dim_h Horizontal dimension of the buffer
 * @param dim_v Vertical dimension of the buffer
 */
void drawText(int x, int y, char* text, unsigned short color, unsigned short* buffer, unsigned int dim_h, unsigned int dim_v);

/**
 * @brief Loads BMP's
 *
 * Creates path to BMP concatenating default path with file name
 * Opens file and reads header information into a HEADER and INFOHEADER struct
 * Reads the pixel colors from the file into an unsigned short array
 *
 * @param filename string with the filename
 * @param width pointer to the width variable to be filled
 * @param height pointer to the height variable to be filled
 *
 * @return the array with the pixel colors and the width and height by reference
 */
unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height);

/**
 * @brief Loads Tool Bar
 *
 * Loads all the buttons from files using loadBMP
 *
 * @param btnArray array of buttons where to load the buttons
 *
 * @return 0 on success, 1 on error
 */
int loadToolBar(BTN* btnArray);

/**
 * @brief Loads Color Bar
 *
 * Loads the color bar from file using loadBMP
 *
 * @param color_bar Pointer to the SPRIE where to load the color bar
 *
 * @return 0 on success, 1 on error
 */
int loadColorBar(SPRITE* color_bar);

/**
 * @brief Loads Cursor
 *
 * Loads cursor from file using loadBMP
 *
 * @return 0 on success, 1 on error
 */
int loadCursor();

/**
 * @brief Loads Char Graphic Library
 *
 * Loads the char graphic library from file using loadBMP
 *
 * @return 0 on success, 1 on error
 */
int loadChars();

/**
 * @brief Loads Intro
 *
 * Loads intro image from file using loadBMP
 *
 * @return 0 on success, 1 on error
 */
int loadIntroGraphics();

/**
 * @brief Frees memory used by Intro
 *
 * Frees graphics used by Intro
 *
 */
void freeIntroGraphics();

/**
 * @brief Loads Menu Graphics
 *
 * Loads graphics used by the menu
 *
 * @return 0 on success, 1 on error
 */
int loadMenuGraphics();

/**
 * @brief Frees memory used by Menu
 *
 * Frees graphics used by Menu
 *
 */
void freeMenuGraphics();

/**
 * @brief Loads Gallery Graphics
 *
 * Loads graphics used by the gallery
 *
 * @return 0 on success, 1 on error
 */
int loadGalleryGraphics();

/**
 * @brief Frees memory used by Gallery
 *
 * Frees graphics used by Gallery
 *
 */
void freeGalleryGraphics();

/**
 * @brief Loads Drawing from file
 *
 * Drawing files only contain the color of the pixels, load everything to array
 *
 * @param number number of the drawing
 * @param draw_screen pointer to draw_screen to fill with the drawing
 *
 * @return 0 on success, 1 on error, image by reference
 */
int loadDrawing(int number, unsigned short* draw_screen);

/**
 * @brief Saves Drawing to file
 *
 * Drawing files only contain the color of the pixels -> saves everything from buffer
 * @param number number of the drawing to save
 * @param buffer that contain the pixels do save
 */
void saveDrawing(int number, unsigned short* buffer);

/** @} */

#endif
