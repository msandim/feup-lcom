#ifndef _GRAPHIC_MODULE_H_
#define _GRAPHIC_MODULE_H_

/** @defgroup Graphic Graphic
 * @{
 *
 * Module to work with graphics
 */

#define DRAW_SCREEN_H 880
#define DRAW_SCREEN_V 650

/** @name Header Struct*/
/**@{
 *
 * Header of BMP file
 */
typedef struct {
	unsigned short int type;                 /* Magic identifier            */
	unsigned int size;                       /* File size in bytes          */
	unsigned short int reserved1, reserved2;
	unsigned int offset;                     /* Offset to image data, bytes */
} HEADER;

/** @}  */

/** @name Info Header Struct*/
/**@{
 *
 * Information Header of BMP file
 */
typedef struct {
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
} INFOHEADER;

/** @}  */

/** @name Sprite Struct*/
/**@{
 *
 * Structure that contains information of images
 */
typedef struct {
	unsigned short* pixels;
	int width, height;
} SPRITE;

/** @}  */

/** @name BTN Struct*/
/**@{
 *
 * Structure that contains information on buttons
 */
typedef struct {
	SPRITE sprite_off;
	SPRITE sprite_on;
	int press_state;
} BTN;

/** @}  */

/** @name Draw Screen Area Struct*/
/**@{
 *
 * Structure that contains information on the drawing portion of the screen
 */
typedef struct
{
  unsigned int h_dim, v_dim;
  unsigned int x_ul_corner, y_ul_corner;
} Draw_screen_area;

/** @}  */

/**
 * @brief Loads Graphics
 *
 * Loads all the graphics needed by the program:
 * 	Characters for printing text
 *	Introduction Screen
 *	Menu Screen
 *	Gallery Screen
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
 * 	Characters for printing text
 *	Introduction Screen
 *	Menu Screen
 *	Gallery Screen
 *	Drawing Screen
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
 * 	Current Drawing
 *	Tool Bar
 *	Color Bar
 *	Mouse
 *
 * @return 0
 */
int set_graphicsDrawMode(unsigned short* draw_screen, Draw_screen_area draw_area, BTN* btn_array, SPRITE color_bar, unsigned short color_selected);

/**
 * @brief Draws Graphics in Intro
 *
 * Prints Image in Intro Screen
 *
 * @return 0
 */
int set_graphicsIntroMode();

/**
 * @brief Draws Graphics in Menu
 *
 * Prints Image in Menu Screen
 *
 * @return 0
 */
int set_graphicsMenuMode();

/**
 * @brief Draws Graphics in Gallery Mode
 *
 * Prints all the graphics in Gallery Mode:
 * 	Current Drawing
 *	Buttons
 *	Mouse
 *
 * @return 0
 */
int set_graphicsGalleryMode();

/**
 * @brief Draws Graphics in Double Buffer
 *
 * Copies buffer of pixels to the Double Buffer
 *
 * @return 0 on success, 1 if buffer does not fit Double Buffer
 */
int drawAreaInDoubleBuffer(unsigned short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v);

/**
 * @brief Checks equality of areas
 *
 * Compares horizontal and vertical dimensions of the areas, as well as positioning
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
int drawMouse();

/**
 * @brief Draws Tool Bar
 *
 * Takes an array of BTN and prints all the buttons, taking in account press states
 */
void drawToolBar(BTN* btnArray, unsigned short* buffer);

/**
 * @brief Draws Text
 *
 * Prints string character by character
 */
void drawText(int x, int y, char* text, unsigned short color, unsigned short* buffer, unsigned int dim_h, unsigned int dim_v);

/**
 * @brief Loads BMP's
 *
 * Creates path to BMP concatenating default path with file name
 * Opens file and reads header information into a HEADER and INFOHEADER struct
 * Reads the pixel colors from the file into an unsigned short array
 *
 * @return the array with the pixel colors and the width and height by reference
 */
unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height);

/**
 * @brief Loads Tool Bar
 *
 * Loads all the buttons from files using loadBMP
 *
 * @return 0 on success, 1 on error
 */
int loadToolBar(BTN* btnArray);

/**
 * @brief Loads Color Bar
 *
 * Loads the color bar from file using loadBMP
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
 * @brief Loads Cursor
 *
 * Loads cursor from file using loadBMP
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
 * Frees array used by Intro
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
 * Frees array used by Menu
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
 * Frees array used by Gallery
 *
 */
void freeGalleryGraphics();

/**
 * @brief Loads Drawing from file
 *
 * Drawing files only contain the color of the pixels, load everything to array
 *
 * @return 0 on success, 1 on error, image by reference
 */
int loadDrawing(int number, unsigned short* draw_screen);

/**
 * @brief Saves Drawing to file
 *
 * Drawing files only contain the color of the pixels, save everything from buffer
 */
void saveDrawing(int number, unsigned short* buffer, unsigned int dim_h, unsigned int dim_v);

/** @} */

#endif
