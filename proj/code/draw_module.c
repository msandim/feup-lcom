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
static Draw_screen_area default_area, current_area;

static BTN* button_array;
static int tool_selected;
static tool_state tool_current_state;

static SPRITE color_bar;
static unsigned short color_selected;
static unsigned int thickness;

static int serial_com_enabled;

void (*tool_handlers[12]) (void) = {
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
    date_draw_handler,
    save_handler
};

unsigned short* getDrawScreen()
{ return draw_screen; }

Draw_screen_area getDrawScreenInfo()
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

  setComDrawScreen(draw_screen);

  // INTERFACE LOADING *********************************
  // ***************************************************

  // Load buttons
  button_array = (BTN*) malloc (12* sizeof(BTN));
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
  for (i=1; i < 13; i++)
  {
    free(button_array[i-1].sprite_on.pixels);
    free(button_array[i-1].sprite_off.pixels);
  }
  free(button_array);

  // Draw Screen (free memory)
  free(draw_screen);
}

void drawModeInit(int enable_serial_com, int file_name)
{
  // START DRAWING MODE **********
  tool_selected = 0;
  color_selected = 0x2104;
  thickness = 5;
  tool_current_state = st0;

  printf("drawModeInit file_name -> %u\n",file_name);

  if (file_name != getTotal())
    loadDrawing(file_name,draw_screen);
  else
  {
    setNumber(getTotal());

    // start as white
    vg_fill_buffer(0xFFFF,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);

    // write a welcome message
    drawText(300,60,"feel free to draw",color_selected,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);
  }

  // all the buttons must be off
  unsigned int i;
  for (i=1; i < 13; i++)
    button_array[i-1].press_state = 0;

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
      getyMousePosition() <= current_area.y_ul_corner + current_area.v_dim - 1 &&
      tool_selected != 0 && tool_selected != 8 && tool_selected != 9 && tool_selected != 11)
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
    case F_MAKECODE: // blank
      button_array[tool_selected].press_state = 0;
      tool_selected = 0;

      blank_handler();

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;

      break;

    case B_MAKECODE: // brush
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 1;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case K_MAKECODE: // bucket (flood fill)
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 2;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case I_MAKECODE: // color picker
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 3;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case C_MAKECODE: // circle
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 4;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case R_MAKECODE: // rectangle
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 5;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case L_MAKECODE: // line
      if (tool_selected != 7 || areasAreEqual(current_area, default_area))
        button_array[tool_selected].press_state = 0;
      tool_selected = 6;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case V_MAKECODE: // area selection
      button_array[tool_selected].press_state = 0;
      tool_selected = 7;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case M_MAKECODE: // mirror effect
      button_array[tool_selected].press_state = 0;
      tool_selected = 8;

      mirror_effect_handler();

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case G_MAKECODE: // magic bucket effect
      button_array[tool_selected].press_state = 0;
      tool_selected = 9;

      magic_bucket_handler();

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;


    case T_MAKECODE: // data stamping
      button_array[tool_selected].press_state = 0;
      tool_selected = 10;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case S_MAKECODE: //save drawing
      button_array[tool_selected].press_state = 0;
      tool_selected = 11;

      save_handler();

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;

      break;

    case ESC_MAKECODE: // ESC (sair) -> go back
    {
      button_array[tool_selected].press_state = 0; // desligar a tool em que estava
      return -1;
    }
    }

    if (getKeyboardLastKey() == PLUS1_MAKECODE || getKeyboardLastKey() == PLUS2_MAKECODE) // "+" pressed
    {
      if (thickness < MAX_THICKNESS)
        thickness++;
    }

    else if (getKeyboardLastKey() == MINUS1_MAKECODE || getKeyboardLastKey() == MINUS2_MAKECODE) // "-" pressed
    {
      if (thickness > MIN_THICKNESS)
        thickness--;
    }

  }

  return 0;
}

void checkCommandUpdate()
{
  unsigned int number_commands_update = MAX_COMMANDS_PER_UPDATE;

  while (number_commands_update)
  {
    if (command_handler())
      return;

    number_commands_update--;
  }
}

void blank_handler()
{
  vg_fill_buffer(0xFFFF,draw_screen,DRAW_SCREEN_H,DRAW_SCREEN_V);

  if (serial_com_enabled) // if we have to send
    sendCommandBlank();
}

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

        //vg_draw_rectangle_buffer(xi - DRAW_SCREENX_UL_CORNER,
            //yi - DRAW_SCREENY_UL_CORNER, xf - xi, yf - yi, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
      }

      tool_current_state = st0;
    }

  }
}

void mirror_effect_handler()
{
  vg_mirror_effect_buffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

  if (serial_com_enabled) // if we have to send
    sendCommandMirrorEffect();
}

void magic_bucket_handler()
{
  vg_magic_bucket_effect_buffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

  if (serial_com_enabled) // if we have to send
    sendCommandMagicBucketEffect();
}

void date_draw_handler()
{
  unsigned int x, y;

  if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
  {
    x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    // get time (attention, it's in BCD)
    Date_info current_rtc_time = getRTCtime();

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

void save_handler()
{
  if (getFileNumber() == getTotal())
    incTotal();

  int number = getFileNumber();
  printf("Number: %u\n",number);
  saveDrawing(number,draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
}
