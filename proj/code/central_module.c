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
#include "gallery_module.h"
#include "timer.h"
#include "time_module.h"


static program_state current_state;

void programInit()
{
  int exitFlag = 0;

  if (screenInit())
  {
    printf("Error starting graphic mode\n");
    screenExit();
    return;
  }

  if (drawModeLoad()) // load the stuff for drawMode
  {
    printf("Error loading drawMode\n");
    screenExit();
    return;
  }

  if (galleryModeLoad())
  {
    printf("Error loading galleryMode\n");
    screenExit();
    return;
  }

  current_state = INTRO; // Start program in intro

  set_graphicsIntroMode(); // Set graphics for intro

  runDevices(); // Run device interrupts

  drawModeFree(); // free the stuff for drawMode

  galleryModeFree(); // free the stuff for galleryMode

  screenExit();
}

void changeProgramState(program_state new_state)
{
  // ter em atencao que se vem da gallery, tenho de passar o nome do ficheiro

  // if we leave draw_multi -> disable serial port
  if (current_state == DRAW_MULTI)
  {
    shutSerialPort();
    printf("porta serie desactivada\n");
  }

  if (new_state == DRAW_SINGULAR)
  {
    if (current_state == GALLERY)
      drawModeInit(0,getFileNumber()); // disable serial com and edit the file we were seeing in the gallery
    else
      drawModeInit(0,getTotalNumberDrawings()); // disable serial com and get a new filename to use
  }

  else if (new_state == DRAW_MULTI)
  {
    initSerialPort();
    printf("porta serie iniciada\n");
    drawModeInit(1,getTotalNumberDrawings()); // enable serial com and get a new filename to use
  }

  else if (new_state == GALLERY)
    galleryModeInit();

  current_state = new_state;
}

void runDevices()
{
  // *** ENABLE MOUSE, KEYBOARD, RTC, TIMER & SERIAL PORT
  int irq_set_mouse = mouse_subscribe_int();
  mouse_send_cmd(ENABLE_PACKETS);
  int irq_set_timer = timer_subscribe_int();
  int irq_set_kbd = keyboard_subscribe_int();
  int irq_set_rtc = rtc_subscribe_int();
  initRTCuieInt();

  int ipc_status;

  message msg;

  unsigned int timer_count = 0;
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
            if (current_state == DRAW_SINGULAR || current_state == DRAW_MULTI)
              mouseDrawEvent();

            else if (current_state == MENU)
            {
              int event_status = mouseMenuEvent();

              if (event_status == 1)
                changeProgramState(DRAW_SINGULAR);

              else if (event_status == 2)
                changeProgramState(DRAW_MULTI);

              else if (event_status == 3)
                changeProgramState(GALLERY);

              else if (event_status == -1)
                exit_flag = 1;
            }

            else if (current_state == GALLERY)
            {
              int event_status = mouseGalleryEvent();

              if (mouseGalleryEvent() == 1)
                changeProgramState(DRAW_SINGULAR);
              else if (mouseGalleryEvent() == -1)
                changeProgramState(MENU);
            }

          }
        }

        if (msg.NOTIFY_ARG & irq_set_kbd) {

          if(updateKeyboardStatus()) // if a valid key is available
          {
            if (current_state == INTRO)
            {
              if (keyboardIntroEvent())
                changeProgramState(MENU);
            }

            else if (current_state == DRAW_SINGULAR || current_state == DRAW_MULTI)
            {
              if (keyboardDrawEvent() == -1) // come back in the states (to menu)
                changeProgramState(MENU);
            }

            else if (current_state == GALLERY)
              keyboardGalleryEvent();

          }
        }

        if (msg.NOTIFY_ARG & irq_set_rtc) {

          updateRTCStatus(); // update hour
        }

        if (msg.NOTIFY_ARG & irq_set_timer) {

          timer_count++;

          if (timer_count%2 == 0){

            if (current_state == DRAW_SINGULAR || current_state == DRAW_MULTI)
              set_graphicsDrawMode(getDrawScreen(),getDrawScreenInfo(),getCurrentDrawScreen(),getButtonArray(),getColorBar(),getColorSelected()); // desenhar tudo

            else if(current_state == MENU)
              set_graphicsMenuMode();

            else if(current_state == GALLERY)
              set_graphicsGalleryMode(getFileDraw());
          }

          if(timer_count%5 == 0)
          {
            if (current_state == DRAW_MULTI)
              checkCommandUpdate();
          }

        }

        break;
      default:
        break;
      }
    } else {

    }
  }

  // ** DISABLE MOUSE, KBD, TIMER, RTC and Serial Port
  mouse_send_cmd(DISABLE_STREAM_MODE);
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  shutRTCuieInt();
  rtc_unsubscribe_int();
}

int keyboardIntroEvent()
{
  if (getKeyboardPressState()) // if a key is pressed, jump screen
    return 1;
  else
    return 0;
}

int mouseMenuEvent()
{
  static int previous_LB_state = 0;

  if (  getyMousePosition() >= 0 &&
      getyMousePosition() < 192 &&
      getMouseLBstate() && !previous_LB_state)
  {
    previous_LB_state = 1;
    return 1; // draw singular mode
  }

  else if (getyMousePosition() >= 192 &&
      getyMousePosition() < 384 &&
      getMouseLBstate() && !previous_LB_state)
  {
    previous_LB_state = 1;
    return 2; // draw multi mode
  }

  else if (getyMousePosition() >= 384 &&
      getyMousePosition() < 576 &&
      getMouseLBstate() && !previous_LB_state)
  {
    previous_LB_state = 1;
    return 3; // gallery mode
  }

  else if (getyMousePosition() >= 576 &&
      getyMousePosition() <= 768 &&
      getMouseLBstate() && !previous_LB_state)
  {
    previous_LB_state = 1;
    return -1; // exit
  }

  previous_LB_state = getMouseLBstate();

  return 0;
}
