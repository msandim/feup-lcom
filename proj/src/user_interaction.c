#include "user_interaction.h"
#include "mouse.h"
#include "video_gr.h"

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


