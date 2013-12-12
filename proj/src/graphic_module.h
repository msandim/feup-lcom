#ifndef _GRAPHIC_MODULE_H_
#define _GRAPHIC_MODULE_H_

int screenInit();

void screenExit();

void drawBufferInVRAM();

int drawAreaInDoubleBuffer(short* buffer, unsigned int x_upperleft_corner, unsigned int y_upperleft_corner,unsigned int dim_h, unsigned int dim_v);

int set_graphicsDrawMode(short* tela);

int draw_mouse();

#endif
