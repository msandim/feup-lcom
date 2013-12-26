#ifndef _GRAPHIC_MODULE_H_
#define _GRAPHIC_MODULE_H_

typedef struct {
	unsigned short int type;                 /* Magic identifier            */
	unsigned int size;                       /* File size in bytes          */
	unsigned short int reserved1, reserved2;
	unsigned int offset;                     /* Offset to image data, bytes */
} HEADER;

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

typedef struct {
	unsigned short* pixels;
	int width, height;
} SPRITE;

typedef struct {
	SPRITE sprite_off;
	SPRITE sprite_on;
	int press_state;
} BTN;

typedef struct
{
  unsigned int h_dim, v_dim;
  unsigned int x_ul_corner, y_ul_corner;
} draw_screen_area;

int screenInit();

void screenExit();

void drawBufferInVRAM();

int set_graphicsDrawMode(unsigned short* draw_screen, draw_screen_area draw_area, BTN* btn_array, SPRITE color_bar, unsigned short color_selected);

int drawAreaInDoubleBuffer(unsigned short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v);

int drawMouse();

void drawToolBar(BTN* btnArray, unsigned short* buffer);

void drawText(int x, int y, char* text, unsigned short color, unsigned short* buffer, unsigned int dim_h, unsigned int dim_v);

unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height);

int loadToolBar(BTN* btnArray);

int loadColorBar(SPRITE* color_bar);

int loadChars();

#endif
