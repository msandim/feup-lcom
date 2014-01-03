#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "user_interaction.h"
#include "mouse.h"
#include "keyboard.h"
#include "video_gr.h"

// KEYBOARD VARIABLES
static unsigned char last_key_code = 0;
static int two_byte_flag = 0, press_key_flag = 0;

/* KEYBOARD STATUS FUNCTIONS */

int updateKeyboardStatus()
{
  unsigned char key_code;

  static int received_E0_before = 0;

  int keyboard_status = keyboard_interrupt_handler(&key_code);

  if (keyboard_status == -1) // error receiving key
    return 0;

  else if (keyboard_status == 0) // normal make code
  {
    last_key_code = key_code;

    if (!received_E0_before) // check if it's a 2 byte code, or 1 byte code
      two_byte_flag = 0;
    else
      two_byte_flag = 1;

    press_key_flag = 1; // because it's a make code
    received_E0_before = 0;
  }

  else if (keyboard_status == 1) // normal break code
  {
    if (!received_E0_before) // check if it's a 2 byte code, or 1 byte code
      two_byte_flag = 0;
    else
      two_byte_flag = 1;

    press_key_flag = 0; // because it's a break code
    received_E0_before = 0;
  }

  else // special initial char (E0)
  {
    received_E0_before = 1;
    return 0;
  }

  return 1;
}

unsigned char getKeyboardLastKey()
{
  return last_key_code;
}

int getKeyboard2ByteCode()
{
  return two_byte_flag;
}

int getKeyboardPressState()
{
  return press_key_flag;
}


// MOUSE VARIABLES
static int x_position=0, y_position=0, RB_pressed=0, MB_pressed=0, LB_pressed=0;

/* MOUSE STATUS FUNCTIONS */

int updateMouseStatus()
{
  mouse_interrupt_handler();

  if (mouse_ended_packet())
  {
    unsigned char * mouse_packet = mouse_get_packet();

    int delta_X=0, delta_Y=0,XSIGN,YSIGN;

    // updates buttons state
    LB_pressed = (mouse_packet[0] & LB_mask);
    MB_pressed = (mouse_packet[0] & MB_mask) >> 2;
    RB_pressed = (mouse_packet[0] & RB_mask) >> 1;

    // updates x and y coordinates
    XSIGN = (mouse_packet[0] & XSIGN_mask) >> 4; // get signs
    YSIGN = (mouse_packet[0] & YSIGN_mask) >> 5;

    if (XSIGN)
      delta_X = -1;
    if (YSIGN)
      delta_Y = -1;

    delta_X = (delta_X << 8) | mouse_packet[1]; // get deltas
    delta_Y = (delta_Y << 8) | mouse_packet[2];
    delta_Y = -delta_Y; // if we consider Y grows if we go down because of video_mem way

    x_position += delta_X; y_position += delta_Y; // update positions

    if (x_position < 0) // correct if x/y positions pass limits
      x_position = 0;
    else if (x_position > vg_get_h_res()-1)
      x_position = vg_get_h_res()-1;

    if (y_position < 0)
      y_position = 0;
    else if (y_position > vg_get_v_res()-1)
      y_position = vg_get_v_res()-1;

    return 1;
  }

  return 0; // has to finish receiving the packet yet
}

unsigned int getxMousePosition()
{ return (unsigned int) x_position; }

unsigned int getyMousePosition()
{ return (unsigned int) y_position; }

int getMouseLBstate()
{ return LB_pressed; }

int getMouseMBstate()
{ return MB_pressed; }

int getMouseRBstate()
{ return RB_pressed; }


