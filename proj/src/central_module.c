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

    // fazer switch da variavel input, se for ESC sai do ecr�

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
  // *** ENABLE MOUSE, KEYBOARD & TIMER
  int irq_set_mouse = mouse_subscribe_int();
  mouse_send_cmd(ENABLE_PACKETS);
  int irq_set_timer = timer_subscribe_int();
  int irq_set_kbd = keyboard_subscribe_int();

  // DRAW SCREEN PREPARATION ****************************
  // ****************************************************
  short* draw_scr = (short*) malloc(DRAW_SCREEN_H * DRAW_SCREEN_V * sizeof(short));

  if (draw_scr == NULL)
  {
    printf("Could not allocate array draw_scr\n");
    return;
  }

  // start as white
  memset(draw_scr,0xFF,DRAW_SCREEN_H * DRAW_SCREEN_V * 2);

  // INTERFACE LOADING *********************************
  // ***************************************************

  // Load buttons
  BTN* btn_array = (BTN*) malloc (11* sizeof(BTN));
  if (loadToolBar(btn_array))
    return;

  // Load color bar
  SPRITE color_bar;
  if (loadColorBar(&color_bar))
    return;

  // migrate into draw mode
  drawMode(irq_set_mouse,irq_set_kbd,irq_set_timer,draw_scr, btn_array, color_bar);

  // Draw Screen (free memory)
  free(draw_scr);

  // ** DISABLE MOUSE, MOUSE & TIMER
  mouse_send_cmd(DISABLE_STREAM_MODE);
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
}
