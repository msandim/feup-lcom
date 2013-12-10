#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "draw_module.h"
#include "graphic_module.h"

typedef enum {st0, st1, st2, st3} tool_state;

static short* draw_screen;

static int tool_selected = 1; // testar com linha continua
static tool_state tool_current_state = st0;

static short color_selected = 3603;

void (*tool_handlers[3]) (void); // 3 ferramentas por agora (null, pincel, linha reta)

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
            //if (getMouseLBstate())
            mouseClickDrawAction();

            if(getMouseMBstate())
              exit_flag = 1;
          }
        }

        if (msg.NOTIFY_ARG & irq_set_timer) {

          timer_count++;

          if (timer_count%2 == 0){

            set_graphicsDrawMode(); // desenhar tudo
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

void mouseClickDrawAction()
{
  /* se o click foi na tela
   if (getxMousePosition() > x && getxMousePosition() < y &&
        getyMousePosition() > w && getyMouse*Position() < z)
        tool_handlers[tool_selected]; */

  /* Se o click foi na tabela das cores */



}

void keyboardDrawAction()
{
  // se mudar de ferramenta:
  //tool_current_state = st0;
}

void null_handler()
{ }

void brush_handler()
{
  static unsigned int last_x, last_y;

  switch(tool_current_state)
  {
  case st0:
    if (getMouseLBstate()) // if the left button is pressed, take note of the coordinates
    {
      last_x = getxMousePosition();
      last_y = getxMousePosition();
      tool_current_state = st1;
    }
    break;

  case st1:
    if (getMouseLBstate()) // while the left button is pressed, draw a line for every movement
    {
      // desenhar linha no buffer

      // colocar novas coordenadas
      last_x = getxMousePosition();
      last_y = getxMousePosition();
      break;
    }
    else
      tool_current_state = st0;
  }
}

void rect_line_handler()
{

  switch(tool_current_state)
  {
  case st0:
    break;
  }
}
