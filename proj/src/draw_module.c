#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <math.h>

#include "draw_module.h"
#include "graphic_module.h"
#include "user_interaction.h"
#include "time_module.h"

#include "com_module.h"

static unsigned short* draw_screen;
static draw_screen_area default_area, current_area;

static char* filename;

static BTN* button_array;
static int tool_selected;
static tool_state tool_current_state;

static SPRITE color_bar;
static unsigned short color_selected;
static unsigned int thickness;

static int serial_com_enabled;

void (*tool_handlers[11]) (void) = {
    blank_handler,
    brush_handler,
    flood_fill_handler,
    color_picker_handler,
    circle_handler,
    rectangle_handler,
    rect_line_handler,
    selected_area_handler,
    mirror_effect_handler,
    magic_bucket_handler,
    date_draw_handler
};

unsigned short* getDrawScreen()
{ return draw_screen; }

draw_screen_area getDrawScreenInfo()
{ return default_area; }

BTN* getButtonArray()
{ return button_array; }

SPRITE getColorBar()
{ return color_bar; }

unsigned short getColorSelected()
{ return color_selected; }

int drawModeLoad()
{
  // DRAW SCREEN PREPARATION ****************************
  // ****************************************************
  draw_screen = (unsigned short*) malloc(DRAW_SCREEN_H * DRAW_SCREEN_V * sizeof(unsigned short));

  if (draw_screen == NULL)
  {
    printf("Could not allocate array draw_scr\n");
    return 1;
  }

  // INTERFACE LOADING *********************************
  // ***************************************************

  // Load buttons
  button_array = (BTN*) malloc (11* sizeof(BTN));
  if (loadToolBar(button_array))
    return 1;

  // Load color bar
  if (loadColorBar(&color_bar))
    return 1;

  // fill variables - default area
  default_area.h_dim = DRAW_SCREEN_H;
  default_area.v_dim = DRAW_SCREEN_V;
  default_area.x_ul_corner = DRAW_SCREENX_UL_CORNER;
  default_area.y_ul_corner = DRAW_SCREENY_UL_CORNER;

  // fill variables - current area
  current_area = default_area;

  return 0;
}

void drawModeFree()
{
  // Color bar (free memory)
  free(color_bar.pixels);

  // tool bar (free memory)
  unsigned int i;
  for (i=1; i < 12; i++)
  {
    free(button_array[i-1].sprite_on.pixels);
    free(button_array[i-1].sprite_off.pixels);
  }
  free(button_array);

  // Draw Screen (free memory)
  free(draw_screen);
}

void drawModeInit(int enable_serial_com)
{
  // start as white
  vg_fill_buffer_white(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

  // START DRAWING MODE **********
  tool_selected = 0;
  color_selected = 0x2104;
  thickness = 5;
  tool_current_state = st0;

  // write a welcome message
  drawText(300,60,"feel free to draw\n",color_selected,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);

  // start with first option selected
  button_array[tool_selected].press_state = 1;

  serial_com_enabled = enable_serial_com;
}

void mouseDrawEvent()
{
  static int previous_LB_state = 0;
  // if the click was on the draw_screen
  if (getxMousePosition() >= current_area.x_ul_corner &&
      getxMousePosition() <= current_area.x_ul_corner + current_area.h_dim - 1 &&
      getyMousePosition() >= current_area.y_ul_corner &&
      getyMousePosition() <= current_area.y_ul_corner + current_area.v_dim - 1)
    (tool_handlers[tool_selected])();

  // if the click was in the color pallet
  else if (getxMousePosition() >= COLOR_BARX_UL_CORNER &&
      getxMousePosition() <= COLOR_BARX_UL_CORNER + COLOR_BAR_H_LENGTH - 1 &&
      getyMousePosition() >= COLOR_BARY_UL_CORNER &&
      getyMousePosition() <= COLOR_BARY_UL_CORNER + COLOR_BAR_V_LENGTH - 1 &&
      getMouseLBstate() && !previous_LB_state)
    color_selected = vg_get_pixel_buffer(getxMousePosition() - 122, getyMousePosition() - 700, color_bar.pixels, 882, 62);


  // if the click was not on a relevant place, disable personalized draw area
  else if (!previous_LB_state && getMouseLBstate()) // if it was a real click
  {
    button_array[7].press_state = 0;
    current_area = default_area;
  }


  previous_LB_state = getMouseLBstate();

}

int keyboardDrawEvent()
{
  if (getKeyboardPressState()) // if a makecode is released and not a breakcode
  {
    switch(getKeyboardLastKey())
    {
    case 0x21: // blank
      button_array[tool_selected].press_state = 0;
      tool_selected = 0;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;

      vg_fill_buffer_white(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      if (serial_com_enabled) // if we have to send
        sendCommandBlank();

      break;

    case 0x30: // pincel
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 1;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x25: // balde
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 2;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x17: // color picker
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 3;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x2E: // circulo
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 4;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x13: // rectangulo
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 5;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x26: // linha
      if (tool_selected != 7)
        button_array[tool_selected].press_state = 0;
      tool_selected = 6;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x2F: // selecao de areas
      button_array[tool_selected].press_state = 0;
      tool_selected = 7;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x14: // stamp
      button_array[tool_selected].press_state = 0;
      tool_selected = 10;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x01: // ESC (sair) -> voltar atrás no estado
    {
      button_array[tool_selected].press_state = 0; // desligar a tool em que estava
      return -1;
    }
    }

    if (getKeyboardLastKey() == 0x4E || getKeyboardLastKey() == 0x1A) // "+" pressed
    {
      if (thickness < MAX_THICKNESS)
        thickness++;
    }

    else if (getKeyboardLastKey() == 0x4A || getKeyboardLastKey() == 0x35) // "-" pressed
    {
      if (thickness > MIN_THICKNESS)
        thickness--;
    }

  }

  return 0;
}

void checkPixelUpdate()
{

  unsigned int number_pixels_update = MAX_COMMANDS_PER_UPDATE;

  while (number_pixels_update)
  {
    if (command_handler(draw_screen))
      return;

    number_pixels_update--;
  }
}

/*
void checkPixelUpdate()
{
  if (serial_com_enabled)
  {


    unsigned int number_pixels_update = MAX_COMMANDS_PER_UPDATE;
    int empty_read = 0;

    while (number_pixels_update)
    {
      unsigned char cod_char[5];
      unsigned char command_string[25];

      empty_read = receiveCommand(cod_char,1);

      if (empty_read == 1 || empty_read == 2)
      {
        //printf("empty\n");
        if (empty_read == 2)
          printf("timeout .....\n");

        return;
      }

      switch(cod_char[0])
      {
      case 0x1: // circulo
      {
        printf("comeca circulo\n");
        receiveCommand(command_string,8);
        printf("resto circulo\n");

        unsigned int x = (unsigned int) (command_string[0] | (((unsigned int) command_string[1]) << 8));
        unsigned int y = (unsigned int) (command_string[2] | (((unsigned int) command_string[3]) << 8));
        unsigned int radius = (unsigned int) (command_string[4] | (((unsigned int) command_string[5]) << 8));
        unsigned short color = (unsigned short) (command_string[6] | (((unsigned short) command_string[7]) << 8));

        printf("Recebi um circulo (%u,%u) r=%u, color=%u\n",x,y,radius,color);

        vg_draw_circle_buffer(x,y,radius,color,draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
        break;
      }

      case 0x2: // rectangulo
      {
        printf("comeca rect\n");
        receiveCommand(command_string,10);

        unsigned int x = (unsigned int) (command_string[0] | (((unsigned int) command_string[1]) << 8));
        unsigned int y = (unsigned int) (command_string[2] | (((unsigned int) command_string[3]) << 8));
        unsigned int dim_h = (unsigned int) (command_string[4] | (((unsigned int) command_string[5]) << 8));
        unsigned int dim_v = (unsigned int) (command_string[6] | (((unsigned int) command_string[7]) << 8));
        unsigned short color = (unsigned short) (command_string[8] | (((unsigned short) command_string[9]) << 8));

        vg_draw_rectangle_buffer(x, y, dim_h, dim_v, color, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
        printf("rect\n");
        break;
      }

      case 0x3: // linha
      {
        printf("comeca lin\n");
        receiveCommand(command_string,11);

        unsigned int xi = (unsigned int) (command_string[0] | (((unsigned int) command_string[1]) << 8));
        unsigned int yi = (unsigned int) (command_string[2] | (((unsigned int) command_string[3]) << 8));
        unsigned int xf = (unsigned int) (command_string[4] | (((unsigned int) command_string[5]) << 8));
        unsigned int yf = (unsigned int) (command_string[6] | (((unsigned int) command_string[7]) << 8));
        unsigned int radius = (unsigned int) command_string[8];
        unsigned short color = (unsigned short) (command_string[9] | (((unsigned short) command_string[10]) << 8));

        vg_draw_brush_buffer(xi, yi, xf, yf, color, radius, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
        printf("linha\n");
        break;
      }

      case 0x4: // flood fill
      {
        printf("comeca flood\n");
        receiveCommand(command_string,6);

        unsigned int x = (unsigned int) (command_string[0] | (((unsigned int) command_string[1]) << 8));
        unsigned int y = (unsigned int) (command_string[2] | (((unsigned int) command_string[3]) << 8));
        unsigned short color = (unsigned short) (command_string[4] | (((unsigned short) command_string[5]) << 8));

        vg_flood_fill_buffer(x, y, color, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
        printf("flood fill\n");
        break;
      }

      case 0x5: // data stamping
      {
        printf("comeca data\n");
        receiveCommand(command_string,12);

        unsigned int x = (unsigned int) (command_string[0] | (((unsigned int) command_string[1]) << 8));
        unsigned int y = (unsigned int) (command_string[2] | (((unsigned int) command_string[3]) << 8));
        unsigned char seconds = command_string[4];
        unsigned char minutes = command_string[5];
        unsigned char hours = command_string[6];
        unsigned char month_day = command_string[7];
        unsigned char month = command_string[8];
        unsigned char year = command_string[9];
        unsigned short color = (unsigned short) (command_string[10] | (((unsigned short) command_string[11]) << 8));

        char string_date[25];

        snprintf(string_date,25,"%02xh %02xm %02xs %02x-%02x-%02x",
            hours, minutes, seconds, month_day, month, year);

        drawText(x,y,string_date,color,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);
        printf("date\n");
        break;
      }

      case 0x6: // blank
      {
        printf("comeca blank\n");
        reset_draw_screen();
        printf("blank\n");
        break;
      }

      default:
        printf("Comando nao reconhecido\n");
        return;
      }

      printf("comando aceite\n");
      number_pixels_update--;
    }

    //printf("O number_pixels_update saiu como: %u\n",number_pixels_update);
  }
}
 */
void blank_handler()
{ }

void brush_handler()
{
  static unsigned int last_x, last_y;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      last_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      last_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;
      tool_current_state = st1;
    }
    break;

  case st1:
  {
    if (!getMouseLBstate()) // if the LB is not pressed, return to state 0
      tool_current_state = st0;
    else
    {
      unsigned int new_x, new_y;

      // calculate relative position of x and y
      new_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      new_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

      vg_draw_brush_buffer(last_x, last_y, new_x, new_y,
          color_selected, thickness, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      if (serial_com_enabled)
        sendCommandLine(last_x, last_y, new_x, new_y, thickness, color_selected);

      // update last coordinates
      last_x = new_x;
      last_y = new_y;
    }

    break;
  }
  }
}

void flood_fill_handler()
{
  unsigned int x, y;

  if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
  {
    x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    //printf("X: %u\n",x);
    //printf("Y: %u\n",y);
    //printf("COLOR: %X\n",old_color);

    //vg_fill_ra(x, y,
    //  color_selected,
    //draw_screen, current_area.h_dim, current_area.v_dim);
    vg_flood_fill_buffer(x, y, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

    if (serial_com_enabled)
      sendCommandFloodFill(x, y, color_selected);
  }
}

void color_picker_handler()
{
  unsigned int x,y;

  if (getMouseLBstate())
  {
    x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    color_selected = vg_get_pixel_buffer(x, y, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
  }
}

void circle_handler()
{
  static unsigned int xi, yi, xf, yf;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      xi = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      yi = getyMousePosition() - DRAW_SCREENY_UL_CORNER;
      tool_current_state = st1;
    }
    break;

  case st1:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates and draw rectangle
    {
      xf = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      yf = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

      // assert that xf > xi and yi > yf
      if (xf < xi) {
        unsigned int tmp = xi;
        xi = xf;
        xf = tmp;
      }

      if (yf < yi) {
        unsigned int tmp = yi;
        yi = yf;
        yf = tmp;
      }

      unsigned int r = (unsigned int) sqrt ( pow ((xf - xi), 2) + pow ((yf - yi), 2));

      //printf ("Xi: %u\nYi: %u\nXf: %u\nYf: %u\nR: %u\n",x_i,y_i,x_f,y_f,r);

      // see if we are working in personalized area, and if we are, check if the circle fits the area
      if ((!areasAreEqual(default_area, current_area) && (xi + DRAW_SCREENX_UL_CORNER + r <= current_area.x_ul_corner + current_area.h_dim)
          && (xi + DRAW_SCREENX_UL_CORNER - r >= current_area.x_ul_corner) && (yi + DRAW_SCREENY_UL_CORNER + r <= current_area.y_ul_corner + current_area.v_dim)
          && (yi + DRAW_SCREENY_UL_CORNER - r >= current_area.y_ul_corner)) || areasAreEqual(default_area, current_area))
      {
        vg_draw_circle_buffer(xi, yi, r, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

        if (serial_com_enabled)
          sendCommandCircle(xi, yi, r, color_selected);
      }

      tool_current_state = st0;
    }
  }

}

void rectangle_handler()
{
  static unsigned int x_i, y_i, x_f, y_f;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      x_i = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      y_i = getyMousePosition() - DRAW_SCREENY_UL_CORNER;
      tool_current_state = st1;
    }
    break;
  case st1:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates and draw rectangle
    {
      x_f = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      y_f = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

      // assert that xf > xi and yi > yf
      if (x_f < x_i) {
        unsigned int tmp = x_i;
        x_i = x_f;
        x_f = tmp;
      }

      if (y_f < y_i) {
        unsigned int tmp = y_i;
        y_i = y_f;
        y_f = tmp;
      }

      vg_draw_rectangle_buffer(x_i,y_i,x_f - x_i,y_f - y_i,color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      if (serial_com_enabled)
        sendCommandRectangle(x_i, y_i, x_f - x_i, y_f - y_i, color_selected);

      tool_current_state = st0;
    }

  }
}

void rect_line_handler()
{
  static unsigned int last_x, last_y;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      last_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      last_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;
      tool_current_state = st1;
    }
    break;

  case st1:
  {
    if (getMouseLBstate())
    {
      unsigned int new_x, new_y;

      // calculate relative position of x and y
      new_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
      new_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

      vg_draw_brush_buffer(last_x, last_y, new_x, new_y,
          color_selected, thickness, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

      if (serial_com_enabled)
        sendCommandLine(last_x, last_y, new_x, new_y, thickness, color_selected);

      tool_current_state = st0;
    }
  }

  break;
  }
}

void selected_area_handler()
{
  static unsigned int xi, yi, xf, yf;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      xi = getxMousePosition();
      yi = getyMousePosition();
      tool_current_state = st1;
    }
    break;

  case st1:
    if (getMouseLBstate()) // if the left key is not pressed, return to state 0
    {
      xf = getxMousePosition();
      yf = getyMousePosition();

      // assert that xf > xi and yi > yf
      if (xf < xi) {
        unsigned int tmp = xi;
        xi = xf;
        xf = tmp;
      }

      if (yf < yi) {
        unsigned int tmp = yi;
        yi = yf;
        yf = tmp;
      }

      if (yi != yf && xi != xf)
      {
        current_area.h_dim = xf - xi;
        current_area.v_dim = yf - yi;
        current_area.x_ul_corner = xi;
        current_area.y_ul_corner = yi;

        vg_draw_rectangle_buffer(xi - DRAW_SCREENX_UL_CORNER,
            yi - DRAW_SCREENY_UL_CORNER, xf - xi, yf - yi, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
      }

      tool_current_state = st0;
    }

  }
}

void mirror_effect_handler()
{

}

void magic_bucket_handler()
{

}

void date_draw_handler()
{
  unsigned int x, y;

  if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
  {
    x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    // get time (attention, it's in BCD)
    date_info current_rtc_time = getRTCtime();

    char string_date[25];

    snprintf(string_date,25,"%02xh %02xm %02xs %02x-%02x-%02x",
        current_rtc_time.hours,current_rtc_time.minutes, current_rtc_time.seconds,
        current_rtc_time.month_day, current_rtc_time.month, current_rtc_time.year);

    //printf("O tempo eh: %s\n",string_date);

    drawText(x,y,string_date,color_selected,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);

    if (serial_com_enabled)
      sendCommandDateDraw(x,y,current_rtc_time,color_selected);
  }
}
