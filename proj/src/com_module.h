#ifndef _COM_MODULE_H_
#define _COM_MODULE_H_

int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned short color);

int sendCommandRectangle(unsigned int x, unsigned int y, unsigned int dim_h, unsigned int dim_v, unsigned short color);

int sendCommandLine(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf, unsigned int radius, unsigned short color);

int sendCommandFloodFill(unsigned int x, unsigned int y, unsigned short color);

int sendCommandDateDraw(unsigned int x, unsigned int y, date_info date, unsigned short color);

int sendCommandBlank();

/*int sendSetPixel(unsigned int x, unsigned int y, unsigned short color);*/

/*int receiveSetPixel(unsigned int *x, unsigned int *y, unsigned short *color);*/

int receiveCommand(unsigned char command[], unsigned int size);

int initSerialPort();

int shutSerialPort();

#endif
