#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <math.h>

#include "draw_module.h"
#include "graphic_module.h"
#include "user_interaction.h"

typedef enum {st0, st1, st2, st3} tool_state;

static unsigned short* draw_screen;
static draw_screen_area default_area, current_area;

static BTN* button_array;
static int tool_selected; // testar com linha continua
static tool_state tool_current_state;

static SPRITE color_bar;
static unsigned short color_selected;
static unsigned int thickness;

void (*tool_handlers[8]) (void) = {
    blank_handler,
    brush_handler,
    flood_fill_handler,
    color_picker_handler,
    circle_handler,
    rectangle_handler,
    rect_line_handler,
    selected_area_handler
};


void drawMode(int irq_set_mouse, int irq_set_kbd, int irq_set_timer, unsigned short* draw_scr,
    BTN* btn_array, SPRITE clr_bar)
{
  // fill variables
  default_area.h_dim = DRAW_SCREEN_H;
  default_area.v_dim = DRAW_SCREEN_V;
  default_area.x_ul_corner = DRAW_SCREENX_UL_CORNER;
  default_area.y_ul_corner = DRAW_SCREENY_UL_CORNER;

  // fill variables - current screen
  current_area = default_area;

  // fill variables - buttons and color bar
  draw_screen = draw_scr;
  button_array = btn_array;
  color_bar = clr_bar;

  // START DRAWING MODE **********
  tool_selected = 0;
  color_selected = 0x2104;
  thickness = 5;
  tool_current_state = st0;

  // start with first option selected
  button_array[tool_selected].press_state = 1;

  int ipc_status;
  message msg;

  int timer_count=0; // count the timer interrupts

  int exit_flag = 0;

  while(!exit_flag)
  {
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:

        if (msg.NOTIFY_ARG & irq_set_mouse) {

          if(updateMouseStatus()) // update mouse status, if packet ended
          {
            mouseClickDrawEvent();

            if(getMouseRBstate())
              exit_flag = 1;
          }
        }

        if (msg.NOTIFY_ARG & irq_set_kbd) {

          if(updateKeyboardStatus()) // if a valid key is available
            keyboardDrawEvent();
        }

        if (msg.NOTIFY_ARG & irq_set_timer) {

          timer_count++;

          if (timer_count%2 == 0){

            set_graphicsDrawMode(draw_screen,default_area,button_array,color_bar,color_selected); // desenhar tudo
          }
        }

        break;
      default:
        break;
      }
    } else {

    }
  }
}

void mouseClickDrawEvent()
{
  static int previous_LB_state = 0;
  // if the click was on the draw_screen
  if (getxMousePosition() >= current_area.x_ul_corner &&
      getxMousePosition() <= current_area.x_ul_corner + current_area.h_dim - 1 &&
      getyMousePosition() >= current_area.y_ul_corner &&
      getyMousePosition() <= current_area.y_ul_corner + current_area.v_dim - 1)
    (tool_handlers[tool_selected])();

  // if the click was in the color pallet
  else if (getxMousePosition() >= 122 && getxMousePosition() <= 122 + 882 - 1 &&
      getyMousePosition() >= 700 && getyMousePosition() <= 700 + 62 - 1 && getMouseLBstate())
      color_selected = vg_get_pixel_buffer(getxMousePosition() - 122, getyMousePosition() - 700, color_bar.pixels, 882, 62);


  // if the click was not on a relevant place, disable personalized draw area
  else if (!previous_LB_state && getMouseLBstate()) // se foi um clique mesmo
    current_area = default_area;


  previous_LB_state = getMouseLBstate();

}

void keyboardDrawEvent()
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

      blank_handler();
      break;

    case 0x30: // pincel
      button_array[tool_selected].press_state = 0;
      tool_selected = 1;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x25: // balde
      button_array[tool_selected].press_state = 0;
      tool_selected = 2;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x17: // color picker
      button_array[tool_selected].press_state = 0;
      tool_selected = 3;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x2E: // circulo
      button_array[tool_selected].press_state = 0;
      tool_selected = 4;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x13: // rectangulo
      button_array[tool_selected].press_state = 0;
      tool_selected = 5;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x26: // linha
      button_array[tool_selected].press_state = 0;
      tool_selected = 6;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x2F:
      button_array[tool_selected].press_state = 0;
      tool_selected = 7;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;
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
}

void blank_handler()
{
  memset(draw_screen,0xFF,current_area.h_dim * current_area.v_dim * 2);
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
    unsigned long old_color = vg_get_pixel_buffer(x,y, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

    //printf("X: %u\n",x);
    //printf("Y: %u\n",y);
    //printf("COLOR: %X\n",old_color);

    //vg_fill_ra(x, y,
    //  color_selected,
    //draw_screen, current_area.h_dim, current_area.v_dim);
    vg_flood_fill_buffer(x,y,old_color,color_selected,draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
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

      unsigned int r = (unsigned int) sqrt ( pow ((x_f - x_i), 2) + pow ((y_f - y_i), 2));

      //printf ("Xi: %u\nYi: %u\nXf: %u\nYf: %u\nR: %u\n",x_i,y_i,x_f,y_f,r);

      vg_draw_circle_buffer(x_i, y_i, r, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

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

      tool_current_state = st0;
    }
  }

  break;
  }
}

void selected_area_handler()
{
  static unsigned int x_i, y_i, x_f, y_f;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      x_i = getxMousePosition();
      y_i = getyMousePosition();
      tool_current_state = st1;
    }
    break;

  case st1:
    if (getMouseLBstate()) // if the left key is not pressed, return to state 0
    {
      x_f = getxMousePosition();
      y_f = getyMousePosition();

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

      if (y_i != y_f && x_i != x_f)
      {
        current_area.h_dim = x_f - x_i;
        current_area.v_dim = y_f - y_i;
        current_area.x_ul_corner = x_i;
        current_area.y_ul_corner = y_i;

        vg_draw_rectangle_buffer(x_i - DRAW_SCREENX_UL_CORNER,
            y_i - DRAW_SCREENY_UL_CORNER, x_f - x_i, y_f - y_i, color_selected, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
      }

      tool_current_state = st0;
    }

  }
}
