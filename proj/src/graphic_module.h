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
	short* pixels;
	int width, height;
} SPRITE;

typedef struct {
	SPRITE sprite_off;
	SPRITE sprite_on;
	int press_state;
} BTN;

int screenInit();

void screenExit();

void drawBufferInVRAM();

int set_graphicsDrawMode(short* tela, BTN* btn_array, SPRITE color_bar);

int drawAreaInDoubleBuffer(short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v);

int draw_mouse();

unsigned short* loadBMP (char const* filename, unsigned int * width, unsigned int * height);

void loadToolBar(BTN* btnArray);

void drawToolBar(BTN* btnArray, short* buffer);

void loadColorBar(SPRITE* color_bar);

#endif
