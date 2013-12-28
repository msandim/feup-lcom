#ifndef _COM_MODULE_H_
#define _COM_MODULE_H_

int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned short color);

/*int sendSetPixel(unsigned int x, unsigned int y, unsigned short color);*/

/*int receiveSetPixel(unsigned int *x, unsigned int *y, unsigned short *color);*/

int receiveCommand(unsigned char command[], unsigned int size);

int initSerialPort();

int shutSerialPort();

#endif
