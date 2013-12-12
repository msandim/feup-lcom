#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include <string.h>

#include "central_module.h"
#include "mouse.h"
#include "user_interaction.h"
#include "graphic_module.h"
#include "draw_module.h"
#include "timer.h"

void menuInit()
{
  int exitFlag = 0;
  unsigned char user_input;

  if (screenInit())
  {
    printf("Error starting graphic mode\n");
    return;
  }

  while (!exitFlag)
  {
    // DRAW THESE PRINTFS
    printf("Welcome to paint - minix style\n");
    printf("Please choose option:\n1. Draw\n2. Library\nEsc. Exit");
    printf("\nteste\n");

    // fazer switch da variavel input, se for ESC sai do ecrã

    // experiencia: iniciar drawMode() e depois sair

    drawModeInit();
    vg_fill(3603);
    sleep(2);
    exitFlag = 1;
  }

  screenExit();
}

void drawModeInit()
{
  // *** ENABLE MOUSE & TIMER
  int irq_set_mouse = mouse_subscribe_int();
  mouse_send_cmd(ENABLE_PACKETS);
  int irq_set_timer = timer_subscribe_int();

  // draw the tool bars, draw the screen where we draw
  //set_graphicsDrawMode();

  // Draw Screen (reserve space)
  short* draw_scr = (short*) malloc(400 * 400 * sizeof(short));

  if (draw_scr == NULL)
  {
    printf("Could not allocate array draw_scr\n");
    return;
  }

  // start as white
  memset(draw_scr,20,DRAW_SCREEN_H * DRAW_SCREEN_V * 2);

  // migrate into draw mode
  drawMode(irq_set_mouse,irq_set_timer,draw_scr);

  // Draw Screen (free memory)
  free(draw_scr);

  // ** DISABLE MOUSE & TIMER
  mouse_send_cmd(DISABLE_STREAM_MODE);
  mouse_unsubscribe_int();
  timer_unsubscribe_int();
}

