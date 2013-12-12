#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "draw_module.h"
#include "graphic_module.h"
#include "user_interaction.h"

typedef enum {st0, st1, st2, st3} tool_state;

static short* draw_screen;

static int tool_selected = 0; // testar com linha continua
static tool_state tool_current_state = st0;

static short color_selected = 3603;

void (*tool_handlers[1]) (void) = {brush_handler} ; // 2 ferramentas por agora (pincel, linha reta)


void drawMode(int irq_set_mouse, int irq_set_timer, short* draw_scr)
{
  draw_screen = draw_scr;

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

        if (msg.NOTIFY_ARG & irq_set_timer) {

          timer_count++;

          if (timer_count%2 == 0){

            set_graphicsDrawMode(draw_screen); // desenhar tudo

            // desenhar tela
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
  if (getxMousePosition() > 500 && getxMousePosition() < 700 &&
      getyMousePosition() > 200 && getyMousePosition() < 400)
    (tool_handlers[tool_selected])();

  /* Se o click foi na tabela das cores */



}

void keyboardDrawEvent()
{
  // se mudar de ferramenta:
  //tool_current_state = st0;
}

void null_handler()
{ }

void brush_handler()
{
  static unsigned int last_x, last_y;

  //printf("ENTROU");

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      last_x = getxMousePosition();
      last_y = getyMousePosition();
      tool_current_state = st1;
    }
    break;

  case st1:
  {
    // calculate relative position of x and y
    unsigned long pos1_x = last_x - DRAW_SCREENX_UL_CORNER;
    unsigned long pos1_y = last_y - DRAW_SCREENY_UL_CORNER;
    unsigned long pos2_x = getxMousePosition() - DRAW_SCREENX_UL_CORNER;
    unsigned long pos2_y = getyMousePosition() - DRAW_SCREENY_UL_CORNER;

    // desenhar linha no buffer
    vg_fill_buffer(7237, draw_screen,DRAW_SCREEN_H, DRAW_SCREEN_V); // DESENHA VERDE PARA TESTAR

    vg_draw_line_buffer(pos1_x, pos1_y, pos2_x, pos2_y,
        0xFFFF, draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V);
    vg_draw_line_buffer(0,0,100,150,0,draw_screen,200,200);

    vg_set_pixel_buffer(0, 50, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(0, 51, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(1, 50, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(2, 51, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(3, 50, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(3, 51, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(4, 50, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(4, 51, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(5, 50, 0, draw_screen, 200,200);
    vg_set_pixel_buffer(5, 51, 0, draw_screen, 200,200);

    // actualizar tela no double buf
    drawAreaInDoubleBuffer(draw_screen, DRAW_SCREEN_H, DRAW_SCREEN_V,
        DRAW_SCREENX_UL_CORNER, DRAW_SCREENY_UL_CORNER);

    // colocar novas coordenadas
    last_x = getxMousePosition();
    last_y = getyMousePosition();

    if (!getMouseLBstate()) // if the LB is not pressed, return to state 0
      tool_current_state = st0;

    break;
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
