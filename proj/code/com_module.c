#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <string.h>

#include "com_module.h"
#include "graphic_module.h"
#include "serial.h"

typedef enum {st0, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10, st11, st12} command_state;

static unsigned long lcr_backup, rate_backup;

static unsigned char received_char;

void (*handlers[5]) (void) = {
    commandCircleHandler,
    commandRectangleHandler,
    commandLineHandler,
    commandFloodFillHandler,
    commandDateDrawHandler
};

static int command_number;
static command_state current_state;

static unsigned short* draw_screen;

void setComDrawScreen(unsigned short* draw)
{
  draw_screen = draw;
}

int command_handler()
{
  unsigned char temp[5];

  if (ser_receive_string_poll_fifo(SER_PORT_COM1,temp,1))
    return 1;
  else
  {
    received_char = temp[0];

    if (received_char == 0xA1)
    {
      command_number = 0;
      current_state = st0;
    }

    else if (received_char == 0xA2)
    {
      command_number = 1;
      current_state = st0;
    }

    else if (received_char == 0xA3)
    {
      command_number = 2;
      current_state = st0;
    }

    else if (received_char == 0xA4)
    {
      command_number = 3;
      current_state = st0;
    }

    else if (received_char == 0xA5)
    {
      command_number = 4;
      current_state = st0;
    }

    else if (received_char == 0xA6)
    {
      command_number = -1;

      vg_fill_buffer(0xFFFF,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);

      current_state = st0;
    }

    else if (received_char == 0xA7)
    {
      command_number = -1;

      vg_mirror_effect_buffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      current_state = st0;
    }

    else if (received_char == 0xA8)
    {
      command_number = -1;

      vg_magic_bucket_effect_buffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      current_state = st0;
    }

    else if (command_number != -1)
      (handlers[command_number])();

  }

  return 0;
}

void commandCircleHandler()
{
  static unsigned char lsb_x, lsb_y, lsb_radius, lsb_color, msb_x, msb_y, msb_radius, msb_color, mmsb_color;

  switch(current_state)
  {
  case st0:
    lsb_x = received_char;
    current_state = st1;
    break;

  case st1:
    msb_x = received_char;
    current_state = st2;
    break;

  case st2:
    lsb_y = received_char;
    current_state = st3;
    break;

  case st3:
    msb_y = received_char;
    current_state = st4;
    break;

  case st4:
    lsb_radius = received_char;
    current_state = st5;
    break;

  case st5:
    msb_radius = received_char;
    current_state = st6;
    break;

  case st6:
    lsb_color = received_char;
    current_state = st7;
    break;

  case st7:
    msb_color = received_char;
    current_state = st8;
    break;

  case st8:
    mmsb_color = received_char;
    current_state = st0;

    unsigned int x = (unsigned int) (lsb_x | (((unsigned int) msb_x) << 8));
    unsigned int y = (unsigned int) (lsb_y | (((unsigned int) msb_y) << 8));
    unsigned int radius = (unsigned int) (lsb_radius | (((unsigned int) msb_radius) << 8));
    unsigned int color = (unsigned int) (lsb_color | (((unsigned int) msb_color) << 8) | (((unsigned int) mmsb_color) << 16));

    // convert arguments to binary
    unsigned char temp[16];

    sprintf(temp, "%x", x);
    sscanf(temp, "%u", &x);

    sprintf(temp, "%x", y);
    sscanf(temp, "%u", &y);

    sprintf(temp, "%x", radius);
    sscanf(temp, "%u", &radius);

    sprintf(temp, "%x", color);
    sscanf(temp, "%d", &color);

    vg_draw_circle_buffer(x,y,radius,color,draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
  }
}

void commandRectangleHandler()
{
  static unsigned char lsb_x, lsb_y, lsb_dim_h, lsb_dim_v, lsb_color, msb_x, msb_y, msb_dim_h, msb_dim_v, msb_color, mmsb_color;

  switch(current_state)
  {
  case st0:
    lsb_x = received_char;
    current_state = st1;
    break;

  case st1:
    msb_x = received_char;
    current_state = st2;
    break;

  case st2:
    lsb_y = received_char;
    current_state = st3;
    break;

  case st3:
    msb_y = received_char;
    current_state = st4;
    break;

  case st4:
    lsb_dim_h = received_char;
    current_state = st5;
    break;

  case st5:
    msb_dim_h = received_char;
    current_state = st6;
    break;

  case st6:
    lsb_dim_v = received_char;
    current_state = st7;
    break;

  case st7:
    msb_dim_v = received_char;
    current_state = st8;
    break;

  case st8:
    lsb_color = received_char;
    current_state = st9;
    break;

  case st9:
    msb_color = received_char;
    current_state = st10;
    break;

  case st10:
    mmsb_color = received_char;
    current_state = st0;

    unsigned int x = (unsigned int) (lsb_x | (((unsigned int) msb_x) << 8));
    unsigned int y = (unsigned int) (lsb_y | (((unsigned int) msb_y) << 8));
    unsigned int dim_h = (unsigned int) (lsb_dim_h | (((unsigned int) msb_dim_h) << 8));
    unsigned int dim_v = (unsigned int) (lsb_dim_v | (((unsigned int) msb_dim_v) << 8));
    unsigned int color = (unsigned int) (lsb_color | (((unsigned int) msb_color) << 8) | (((unsigned int) mmsb_color) << 16));

    // convert arguments to binary
    unsigned char temp[16];

    sprintf(temp, "%x", x);
    sscanf(temp, "%u", &x);

    sprintf(temp, "%x", y);
    sscanf(temp, "%u", &y);

    sprintf(temp, "%x", dim_h);
    sscanf(temp, "%u", &dim_h);

    sprintf(temp, "%x", dim_v);
    sscanf(temp, "%u", &dim_v);

    sprintf(temp, "%x", color);
    sscanf(temp, "%u", &color);

    vg_draw_rectangle_buffer(x, y, dim_h, dim_v, color, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
  }
}

void commandLineHandler()
{
  static unsigned char lsb_x1, lsb_y1, lsb_x2, lsb_y2, lsb_color, lsb_radius, msb_x1, msb_y1, msb_x2, msb_y2, msb_color, mmsb_color, msb_radius;

  switch(current_state)
  {
  case st0:
    lsb_x1 = received_char;
    current_state = st1;
    break;

  case st1:
    msb_x1 = received_char;
    current_state = st2;
    break;

  case st2:
    lsb_y1 = received_char;
    current_state = st3;
    break;

  case st3:
    msb_y1 = received_char;
    current_state = st4;
    break;

  case st4:
    lsb_x2 = received_char;
    current_state = st5;
    break;

  case st5:
    msb_x2 = received_char;
    current_state = st6;
    break;

  case st6:
    lsb_y2 = received_char;
    current_state = st7;
    break;

  case st7:
    msb_y2 = received_char;
    current_state = st8;
    break;

  case st8:
    lsb_radius = received_char;
    current_state = st9;
    break;

  case st9:
    msb_radius = received_char;
    current_state = st10;
    break;

  case st10:
    lsb_color = received_char;
    current_state = st11;
    break;

  case st11:
    msb_color = received_char;
    current_state = st12;
    break;

  case st12:
    mmsb_color = received_char;
    current_state = st0;

    unsigned int x1 = (unsigned int) (lsb_x1 | (((unsigned int) msb_x1) << 8));
    unsigned int x2 = (unsigned int) (lsb_x2 | (((unsigned int) msb_x2) << 8));
    unsigned int y1 = (unsigned int) (lsb_y1 | (((unsigned int) msb_y1) << 8));
    unsigned int y2 = (unsigned int) (lsb_y2 | (((unsigned int) msb_y2) << 8));
    unsigned int radius = (unsigned int) (lsb_radius | (((unsigned int) msb_radius) << 8));
    unsigned int color = (unsigned int) (lsb_color | (((unsigned int) msb_color) << 8) | (((unsigned int) mmsb_color) << 16));

    // convert arguments to binary
    unsigned char temp[16];

    sprintf(temp, "%x", x1);
    sscanf(temp, "%u", &x1);

    sprintf(temp, "%x", x2);
    sscanf(temp, "%u", &x2);

    sprintf(temp, "%x", y1);
    sscanf(temp, "%u", &y1);

    sprintf(temp, "%x", y2);
    sscanf(temp, "%u", &y2);

    sprintf(temp, "%x", radius);
    sscanf(temp, "%u", &radius);

    sprintf(temp, "%x", color);
    sscanf(temp, "%u", &color);

    vg_draw_brush_buffer(x1, y1, x2, y2, color, radius, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
  }
}

void commandFloodFillHandler()
{
  static unsigned char lsb_x, lsb_y, lsb_color, msb_x, msb_y, msb_color, mmsb_color;

  switch(current_state)
  {
  case st0:
    lsb_x = received_char;
    current_state = st1;
    break;

  case st1:
    msb_x = received_char;
    current_state = st2;
    break;

  case st2:
    lsb_y = received_char;
    current_state = st3;
    break;

  case st3:
    msb_y = received_char;
    current_state = st4;
    break;

  case st4:
    lsb_color = received_char;
    current_state = st5;
    break;

  case st5:
    msb_color = received_char;
    current_state = st6;
    break;

  case st6:
    mmsb_color = received_char;
    current_state = st0;

    unsigned int x = (unsigned int) (lsb_x | (((unsigned int) msb_x) << 8));
    unsigned int y = (unsigned int) (lsb_y | (((unsigned int) msb_y) << 8));
    unsigned int color = (unsigned int) (lsb_color | (((unsigned int) msb_color) << 8) | (((unsigned int) mmsb_color) << 16));

    // convert arguments to binary
    unsigned char temp[16];

    sprintf(temp, "%x", x);
    sscanf(temp, "%u", &x);

    sprintf(temp, "%x", y);
    sscanf(temp, "%u", &y);

    sprintf(temp, "%x", color);
    sscanf(temp, "%u", &color);

    vg_flood_fill_buffer(x, y, color, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
  }
}

void commandDateDrawHandler()
{
  static unsigned char lsb_x, lsb_y, lsb_color, msb_x, msb_y, msb_color, mmsb_color;
  static unsigned char seconds, minutes, hours, day, month, year;

  switch(current_state)
  {
  case st0:
    lsb_x = received_char;
    current_state = st1;
    break;

  case st1:
    msb_x = received_char;
    current_state = st2;
    break;

  case st2:
    lsb_y = received_char;
    current_state = st3;
    break;

  case st3:
    msb_y = received_char;
    current_state = st4;
    break;

  case st4:
    seconds = received_char;
    current_state = st5;
    break;

  case st5:
    minutes = received_char;
    current_state = st6;
    break;

  case st6:
    hours = received_char;
    current_state = st7;
    break;

  case st7:
    day = received_char;
    current_state = st8;
    break;

  case st8:
    month = received_char;
    current_state = st9;
    break;

  case st9:
    year = received_char;
    current_state = st10;
    break;

  case st10:
    lsb_color = received_char;
    current_state = st11;
    break;

  case st11:
    msb_color = received_char;
    current_state = st12;
    break;

  case st12:
    mmsb_color = received_char;
    current_state = st0;

    unsigned int x = (unsigned int) (lsb_x | (((unsigned int) msb_x) << 8));
    unsigned int y = (unsigned int) (lsb_y | (((unsigned int) msb_y) << 8));
    unsigned int color = (unsigned int) (lsb_color | (((unsigned int) msb_color) << 8) | (((unsigned int) mmsb_color) << 16));

    char string_date[25];

    snprintf(string_date,25,"%02xh %02xm %02xs %02x-%02x-%02x",
        hours, minutes, seconds, day, month, year);

    // convert arguments to binary
    unsigned char temp[16];

    sprintf(temp, "%x", x);
    sscanf(temp, "%u", &x);

    sprintf(temp, "%x", y);
    sscanf(temp, "%u", &y);

    sprintf(temp, "%x", color);
    sscanf(temp, "%u", &color);

    drawText(x,y,string_date,color,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);
  }
}

int sendCommandCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned long color)
{
  unsigned char command[25];
  char temp[16];

  // convert arguments to BCD (hex)
  sprintf(temp, "%u", x);
  sscanf(temp, "%x", &x);

  sprintf(temp, "%u", y);
  sscanf(temp, "%x", &y);

  sprintf(temp, "%u", radius);
  sscanf(temp, "%x", &radius);

  sprintf(temp, "%u", color);
  sscanf(temp, "%x", &color);

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_radius = (unsigned char) (radius & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_radius = (unsigned char) (radius >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);
  unsigned char mmsb_color = (unsigned char) (color >> 16); // because the color can have 5 digits!

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c",0xA1,lsb_x,msb_x,lsb_y,msb_y,lsb_radius,msb_radius,lsb_color,msb_color,mmsb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,10);
}


int sendCommandRectangle(unsigned int x, unsigned int y, unsigned int dim_h, unsigned int dim_v, unsigned long color)
{
  unsigned char command[25];
  unsigned char temp[16];

  // convert arguments to BCD (hex)
  sprintf(temp, "%u", x);
  sscanf(temp, "%x", &x);

  sprintf(temp, "%u", y);
  sscanf(temp, "%x", &y);

  sprintf(temp, "%u", dim_h);
  sscanf(temp, "%x", &dim_h);

  sprintf(temp, "%u", dim_v);
  sscanf(temp, "%x", &dim_v);

  sprintf(temp, "%u", color);
  sscanf(temp, "%x", &color);

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
  unsigned char mmsb_color = (unsigned char) (color >> 16); // because the color can have 5 digits!

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c%c",0xA2,lsb_x, msb_x, lsb_y, msb_y,
      lsb_dim_h, msb_dim_h, lsb_dim_v, msb_dim_v, lsb_color, msb_color, mmsb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,12);
}

int sendCommandLine(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf, unsigned int radius, unsigned long color)
{
  unsigned char command[25];
  unsigned char temp[16];

  // convert arguments to BCD (hex)
  sprintf(temp, "%u", xi);
  sscanf(temp, "%x", &xi);

  sprintf(temp, "%u", xf);
  sscanf(temp, "%x", &xf);

  sprintf(temp, "%u", yi);
  sscanf(temp, "%x", &yi);

  sprintf(temp, "%u", yf);
  sscanf(temp, "%x", &yf);

  sprintf(temp, "%u", radius);
  sscanf(temp, "%x", &radius);

  sprintf(temp, "%u", color);
  sscanf(temp, "%x", &color);

  unsigned char lsb_xi = (unsigned char) (xi & 0xFF);
  unsigned char lsb_yi = (unsigned char) (yi & 0xFF);
  unsigned char lsb_xf = (unsigned char) (xf & 0xFF);
  unsigned char lsb_yf = (unsigned char) (yf & 0xFF);
  unsigned char lsb_radius = (unsigned char) (radius & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024, that radius (thickness) < 200
  unsigned char msb_xi = (unsigned char) (xi >> 8);
  unsigned char msb_yi = (unsigned char) (yi >> 8);
  unsigned char msb_xf = (unsigned char) (xf >> 8);
  unsigned char msb_yf = (unsigned char) (yf >> 8);
  unsigned char msb_radius = (unsigned char) (radius >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);
  unsigned char mmsb_color = (unsigned char) (color >> 16); // because the color can have 5 digits!

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c",0xA3,lsb_xi, msb_xi, lsb_yi, msb_yi,
      lsb_xf, msb_xf, lsb_yf, msb_yf, lsb_radius, msb_radius, lsb_color, msb_color, mmsb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,14);
}

int sendCommandFloodFill(unsigned int x, unsigned int y, unsigned long color)
{
  unsigned char command[25];
  unsigned char temp[16];

  // convert arguments to BCD (hex)
  sprintf(temp, "%u", x);
  sscanf(temp, "%x", &x);

  sprintf(temp, "%u", y);
  sscanf(temp, "%x", &y);

  sprintf(temp, "%u", color);
  sscanf(temp, "%x", &color);

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024,
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);
  unsigned char mmsb_color = (unsigned char) (color >> 16); // because the color can have 5 digits!

  snprintf(command,25,"%c%c%c%c%c%c%c%c",0xA4,lsb_x, msb_x, lsb_y, msb_y, lsb_color, msb_color, mmsb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,8);
}

int sendCommandDateDraw(unsigned int x, unsigned int y, Date_info date, unsigned long color)
{
  unsigned char command[25];
  unsigned char temp[16];

  // convert arguments to BCD (hex)
  sprintf(temp, "%u", x);
  sscanf(temp, "%x", &x);

  sprintf(temp, "%u", y);
  sscanf(temp, "%x", &y);

  sprintf(temp, "%u", color);
  sscanf(temp, "%x", &color);

  unsigned char lsb_x = (unsigned char) (x & 0xFF);
  unsigned char lsb_y = (unsigned char) (y & 0xFF);
  unsigned char lsb_color = (unsigned char) (color & 0xFF);

  // we are certain that x and y < 1024,
  unsigned char msb_x = (unsigned char) (x >> 8);
  unsigned char msb_y = (unsigned char) (y >> 8);
  unsigned char msb_color = (unsigned char) (color >> 8);
  unsigned char mmsb_color = (unsigned char) (color >> 16); // because the color can have 5 digits!

  snprintf(command,25,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0xA5, lsb_x, msb_x, lsb_y, msb_y,
      date.seconds, date.minutes, date.hours, date.month_day, date.month, date.year, lsb_color, msb_color, mmsb_color);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,14);
}

int sendCommandBlank()
{
  unsigned char command[25];

  snprintf(command,25,"%c",0xA6);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,1);
}

int sendCommandMirrorEffect()
{
  unsigned char command[25];

  snprintf(command,25,"%c",0xA7);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,1);
}

int sendCommandMagicBucketEffect()
{
  unsigned char command[25];

  snprintf(command,25,"%c",0xA8);

  return ser_send_string_poll_fifo(SER_PORT_COM1,command,1);
}

int initSerialPort()
{
  // copy registers for backup
  if (ser_get_reg(SER_PORT_COM1,UART_LCR,&lcr_backup))
    return 1;
  if (ser_get_bit_rate(SER_PORT_COM1,&rate_backup))
    return 1;

  // configs
  if (ser_set_config(SER_PORT_COM1, 8, 1, 0, DEFAULT_BITRATE))
    return 1;

  // init fifos
  ser_init_fifo_poll(SER_PORT_COM1);

  command_number = -1;
  current_state = st0;

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

  return 0;
}
