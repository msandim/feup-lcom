#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "draw_module.h"
#include "graphic_module.h"
#include "user_interaction.h"

typedef enum {st0, st1, st2, st3} tool_state;

static short* draw_screen;

static BTN* button_array;
static int tool_selected; // testar com linha continua
static tool_state tool_current_state;

static SPRITE color_bar;
static short color_selected = 3603;

void (*tool_handlers[3]) (void) = {
    blank_handler,
    brush_handler,
    rectangle_handler
};


void drawMode(int irq_set_mouse, int irq_set_kbd, int irq_set_timer, short* draw_scr,
    BTN* btn_array, SPRITE clr_bar)
{
  // fill variables
  draw_screen = draw_scr;
  button_array = btn_array;
  color_bar = clr_bar;

  // START DRAWING MODE **********
  tool_selected = 0;
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
            if (getMouseLBstate())
              mouseClickDrawEvent();

            if(getMouseMBstate())
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

            set_graphicsDrawMode(draw_screen,button_array,color_bar); // desenhar tudo
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
  // if the click was on the draw_screen
  if (getxMousePosition() >= 122 && getxMousePosition() <= 1001 &&
      getyMousePosition() >= 30 && getyMousePosition() <= 680)
    (tool_handlers[tool_selected])();

  /* Se o click foi na tabela das cores */

}

void keyboardDrawEvent()
{
  if (getKeyboardPressState())
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
      //tool_selected = 2;
      tool_current_state = st0;
      break;

    case 0x17: // conta-gotas
      //tool_selected = 3;
      tool_current_state = st0;
      break;

    case 0x2E: // circulo
      //tool_selected = 4;
      tool_current_state = st0;
      break;

    case 0x13: // rectangulo
      button_array[tool_selected].press_state = 0;
      tool_selected = 2;

      button_array[tool_selected].press_state = 1;
      tool_current_state = st0;
      break;

    case 0x26: // linha
      //tool_selected = 6;
      tool_current_state = st0;
      break;
    }
  }
}

void blank_handler()
{
  memset(draw_screen,0xFF,DRAW_SCREEN_H * DRAW_SCREEN_V * 2);
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
    unsigned int new_x, new_y;

    // calculate relative position of x and y
    new_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    new_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    vg_draw_line_buffer(last_x, last_y, new_x, new_y,
        0, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);

    // update draw screen
    drawAreaInDoubleBuffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V,
        DRAW_SCREENX_UL_CORNER, DRAW_SCREENY_UL_CORNER);

    // update last coordinates
    last_x = new_x;
    last_y = new_y;

    if (!getMouseLBstate()) // if the LB is not pressed, return to state 0
      tool_current_state = st0;

    break;
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

      vg_draw_rectangle_buffer(x_i,y_i,x_f-x_i,y_f-y_i,0xFF, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
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
    break;
  }
}
