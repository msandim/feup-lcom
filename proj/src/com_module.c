#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <string.h>

#include "time_module.h"
#include "serial.h"

static unsigned long lcr_backup, rate_backup;

int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned short color)
{
  unsigned char command[25];

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_radius = (unsigned char) (radius & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_radius = (unsigned char) (radius >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c",0x01,lsb_x,msb_x,lsb_y,msb_y,lsb_radius,msb_radius,lsb_color,msb_color);

  //printf("Enviei um circulo (%u,%u), r=%u, color=%u\n",x,y,radius,color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,9);
}

int sendCommandRectangle(unsigned int x, unsigned int y, unsigned int dim_h, unsigned int dim_v, unsigned short color)
{
  unsigned char command[25];

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_dim_h = (unsigned char) (dim_h & 0xFF);
  unsigned char lsb_dim_v = (unsigned char) (dim_v & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_dim_h = (unsigned char) (dim_h >> 8);
  unsigned char msb_dim_v = (unsigned char) (dim_v >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c",0x02,lsb_x, msb_x, lsb_y, msb_y,
      lsb_dim_h, msb_dim_h, lsb_dim_v, msb_dim_v, lsb_color, msb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,11);
}

int sendCommandLine(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf, unsigned int radius, unsigned short color)
{
  unsigned char command[25];

  unsigned char lsb_xi = (unsigned char) (xi & 0xFF);
  unsigned char lsb_yi = (unsigned char) (yi & 0xFF);
  unsigned char lsb_xf = (unsigned char) (xf & 0xFF);
  unsigned char lsb_yf = (unsigned char) (yf & 0xFF);
  unsigned char lsb_only_radius = (unsigned char) (radius & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024, that radius (thickness) < 200
  unsigned char msb_xi = (unsigned char) (xi >> 8);
  unsigned char msb_yi = (unsigned char) (yi >> 8);
  unsigned char msb_xf = (unsigned char) (xf >> 8);
  unsigned char msb_yf = (unsigned char) (yf >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c%c",0x03,lsb_xi, msb_xi, lsb_yi, msb_yi,
      lsb_xf, msb_xf, lsb_yf, msb_yf, lsb_only_radius, lsb_color, msb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,12);
}

int sendCommandFloodFill(unsigned int x, unsigned int y, unsigned short color)
{
  unsigned char command[25];

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024,
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);

  snprintf(command,25,"%c%c%c%c%c%c%c",0x04,lsb_x, msb_x, lsb_y, msb_y, lsb_color, msb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,7);
}

int sendCommandDateDraw(unsigned int x, unsigned int y, date_info date, unsigned short color)
{
  unsigned char command[25];

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024,
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x05, lsb_x, msb_x, lsb_y, msb_y,
      date.seconds, date.minutes, date.hours, date.month_day, date.month, date.year, lsb_color, msb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,13);
}

int sendCommandBlank()
{
  unsigned char command[25];

  snprintf(command,25,"%c",0x06);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,1);
}

/*
int sendSetPixel(unsigned int x, unsigned int y, unsigned short color)
{
  unsigned char pixel_string[7], msb_x, msb_y, msb_color, lsb_x, lsb_y, lsb_color;

  lsb_x = (x & 0xFF);
  lsb_y = (y & 0xFF);
  lsb_color = (color & 0xFF);

  // we are certain that x and y < 1024
  msb_x = (unsigned char) (x >> 8);
  msb_y = (unsigned char) (y >> 8);
  msb_color = (unsigned char) (color >> 8);

  snprintf(pixel_string,7,"%c%c%c%c%c%c",lsb_x,msb_x,lsb_y,msb_y,lsb_color,msb_color);

  // mandar pixel_string
  return ser_send_string_poll_fifo(SER_PORT_COM1,pixel_string);
  //return 0;
}
 */
int receiveCommand(unsigned char command[], unsigned int size)
{
  return (ser_receive_string_poll_fifo(SER_PORT_COM1,command,size));
}
/*
int receiveSetPixel(unsigned int *x, unsigned int *y, unsigned short *color)
{
  unsigned char pixel_string[7];

  if (ser_receive_string_poll_fifo(SER_PORT_COM1,pixel_string))
    return 1;

 *x = (unsigned int) (pixel_string[0] | ((unsigned int) pixel_string[1] << 8));
 *y = (unsigned int) (pixel_string[2] | ((unsigned int) pixel_string[3] << 8));
 *color = (unsigned short) (pixel_string[4] | ((unsigned short) pixel_string[5] << 8));

  return 0;
}
 */
int initSerialPort()
{
  // copy registers for backup
  if (ser_get_reg(SER_PORT_COM1,UART_LCR,&lcr_backup))
    return 1;
  if (ser_get_bit_rate(SER_PORT_COM1,&rate_backup))
    return 1;

  // configs
  if (ser_set_config(SER_PORT_COM1, 8, 1, 0, 9600))
    return 1;

  unsigned long big_ratty;

  ser_get_bit_rate(SER_PORT_COM1,&big_ratty);

  printf("RATE 96 muheheh com 700 por cada 10 gurl: %u\n",big_ratty);

  // init fifos
  ser_init_fifo_poll(SER_PORT_COM1);


  printf("FIFO's activados com sucesso!\n");
  return 0;
}

int shutSerialPort()
{
  // shutdown fifos
  ser_shut_fifo_poll(SER_PORT_COM1);

  // reset lcr and rate previously saved
  if (ser_set_reg(SER_PORT_COM1,UART_LCR,lcr_backup))
    return 1;

  if (ser_set_bit_rate(SER_PORT_COM1,rate_backup))
    return 1;

  printf("FIFO's desactivados com sucesso!\n");

  return 0;
}
