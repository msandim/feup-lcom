#ifndef _COM_MODULE_H_
#define _COM_MODULE_H_

#include "time_module.h"

int command_handler(unsigned short* draw);

void commandCircleHandler();

void commandRectangleHandler();

void commandLineHandler();

void commandFloodFillHandler();

void commandDateDrawHandler();

void commandBlankHandler();

int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned long color);

int sendCommandRectangle(unsigned int x, unsigned int y, unsigned int dim_h, unsigned int dim_v, unsigned long color);

int sendCommandLine(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf, unsigned int radius, unsigned long color);

int sendCommandFloodFill(unsigned int x, unsigned int y, unsigned long color);

int sendCommandDateDraw(unsigned int x, unsigned int y, date_info date, unsigned long color);

int sendCommandBlank();

int receiveCommand(unsigned char command[], unsigned int size);

int initSerialPort();

int shutSerialPort();

#endif
