#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "central_module.h"
#include "mouse.h"
#include "user_interaction.h"
#include "graphic_module.h"
#include "timer.h"

void menuInit()
{
  int exitFlag = 0;
  unsigned char user_input;

  screenInit();

  while (!exitFlag)
  {
    // DRAW THESE PRINTFS
    printf("Welcome to paint - minix style");
    printf("Please choose option:\n1. Draw\n2. Library\nEsc. Exit");

    // fazer switch da variavel input, se for ESC sai do ecrã

    // experiencia: iniciar drawMode() e depois sair

    drawMode();
    vg_fill(3603);
    sleep(2);
    exitFlag = 1;
  }

  screenExit();
}

void drawMode()
{
  int ipc_status;
  message msg;

  // *** ENABLE MOUSE
  int irq_set_mouse = mouse_subscribe_int();
  mouse_send_cmd(ENABLE_PACKETS);

  // *** ENABLE TIMER FOR FRAME CONTROL
  int irq_set_timer = timer_subscribe_int();

  int timer_count=0; // count the timer interrupts

  int exit_flag = 0;

  // draw the tool bars, draw the screen where we draw
  set_drawMode();

  while(!exit_flag)
  {
    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */

        if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */

          mouse_interrupt_handler(); // update mouse status

          if (mouse_ended_packet() && mouse_left_button()) // if we a new valid update on the mouse and we press left button, exit
              exit_flag = 1;
        }

        if (msg.NOTIFY_ARG & irq_set_timer) {

          timer_count++;

          if (timer_count%2 == 0){
            //printf("timer_count: %u\n",timer_count);

            set_drawMode();
          }
        }

        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  // disable stream mode
  mouse_send_cmd(DISABLE_STREAM_MODE);

  timer_unsubscribe_int();
  mouse_unsubscribe_int();
}
