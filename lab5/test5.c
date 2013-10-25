#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test5.h"

static int hook_id;

unsigned char packet[3];

unsigned short count=0;

typedef enum {stateInit, stateLeft, stateRight} state_mouse;
// for the states

// ********************************************
// TEST FUNCTIONS
// ********************************************
int test_packet() {
  int ipc_status;
  message msg;


  // know what ints are we interested in
  int irq_set = mouse_subscribe_int();

  mouse_send_cmd(ENABLE_PACKETS);

  int exit_flag = 0;

  while( exit_flag == 0 ) {

    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          mouse_interrupt_handler();

          if (count == 0 && ((packet[0] & bit3_mask) == bit3_mask)){
            if (mouse_exit_handler()){
              printf("User stopped process\n");
              exit_flag = 1;
            }
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

  mouse_unsubscribe_int();

  // disable stream mode
  mouse_send_cmd(DISABLE_STREAM_MODE);


  return 0;

}

int test_asynch(unsigned short duration) {

  int ipc_status;
  message msg;

  // know what ints are we interested in
  int irq_set_mouse = mouse_subscribe_int();
  int irq_set_timer = timer_subscribe_int();

  mouse_send_cmd(ENABLE_PACKETS);

  int int_count = 0, timer_seconds = 0;

  while( timer_seconds < duration ) {

    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */

        if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */

          mouse_interrupt_handler();

        }

        if (msg.NOTIFY_ARG & irq_set_timer) {

          int_count ++;

          if (int_count%60 == 0){
            int_count = 0;
            timer_seconds++;
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

  timer_unsubscribe_int();
  mouse_unsubscribe_int();

  // clean outbuf -> in the worst case the timer stops after he received 1 byte ..
  // we have to clean 2 times!
  unsigned char garbage;

  mouse_receive_data_outbuf(&garbage);
  mouse_receive_data_outbuf(&garbage);

  // disable stream mode
  mouse_send_cmd(DISABLE_STREAM_MODE);

  return 0;
}
int test_config(void) {

  mouse_subscribe_int();

  unsigned char byte1, byte2, byte3;

  if (mouse_get_config(&byte1, &byte2, &byte3))
  {
    mouse_unsubscribe_int();
    return 1;
  }
  mouse_show_config(byte1,byte2,byte3);

  mouse_unsubscribe_int();

  return 0;
}

// ********************************************************
// MOUSE FUNCTIONS
// ********************************************************

int mouse_subscribe_int()
{
  // set hook_id with the bit to put to one
  hook_id = MOUSE_IRQ;

  // create a bit_mask with the int active
  int bit_mask = BIT(hook_id);

  // use both policies
  int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;

  // hook_id will be changed with a different ID, but no problem
  // because we have the bit_mask already :-)
  if (sys_irqsetpolicy(MOUSE_IRQ,policy,&hook_id) != OK ||
      sys_irqenable(&hook_id) != OK)
    return -1;

  return bit_mask;
}

int mouse_unsubscribe_int()
{
  if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  else
    return 0;
}

int mouse_send_cmd(unsigned long cmd)
{
  unsigned long stat;

  unsigned long timeoutcounter = 0;

  while(timeoutcounter < DELAY_US*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return 1;

    /* loop while 8042 input buffer is not empty */
    if( (stat & IBF) == 0 ) { // if the IBF bit is 0!

      if (sys_outb(KBC_CMD_REG ,WRITE_BYTE_MOUSE) != OK || sys_outb(MOUSE_CMD_PORT, cmd) != OK)
      {
        printf("mouse.c::Failed sending the command %X - %X to mouse\n",WRITE_BYTE_MOUSE,cmd);
        return 1;
      }

      unsigned char info;

      mouse_receive_data_outbuf(&info);

      if (info == ACK)
      {
        // SUCCESS
        tickdelay(micros_to_ticks(DELAY_US));
        return 0;
      }
    }

    // if the input buffer is not empty, lets check again
    // Or .. if non-ACK
    tickdelay(micros_to_ticks(DELAY_US));

    timeoutcounter += DELAY_US;
  }

  return 1;
}

int mouse_receive_data_outbuf(unsigned char *data)
{
  unsigned long stat, data_long;

  unsigned long timeoutcounter = 0;

  while( timeoutcounter < DELAY_US*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return 1;

    //only receive if OBF is 1 -> out_buffer is full
    if( stat & OBF ) {
      if (sys_inb(OUT_BUF, &data_long) != OK)
        return 1;

      *data = (unsigned char) data_long;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    timeoutcounter += DELAY_US;
  }
  return 1;
}

void mouse_interrupt_handler()
{
  unsigned char code;

  if (mouse_receive_data_outbuf(&code))
  {
    printf("Didnt receive data from mouse yet. Try to move the mouse\n\n");
    return;
  }

  if (count == 0 && ((code & bit3_mask) == bit3_mask))
  {
    packet[count] = code;
    count++;
  }
  else if (count == 1)
  {
    packet[count] = code;
    count++;
  }
  else if (count == 2)
  {
    packet[count] = code;
    count = 0; // initializes
    mouse_print_packet(); // prints packet
  }
}

int mouse_get_config(unsigned char *byte1, unsigned char *byte2, unsigned char *byte3)
{
  // temp variables
  unsigned char byte1_temp, byte2_temp, byte3_temp;

  if (
      // request the 3 config bytes
      mouse_send_cmd(STATUS_REQUEST) ||

      mouse_receive_data_outbuf(&byte1_temp) ||
      mouse_receive_data_outbuf(&byte2_temp) ||
      mouse_receive_data_outbuf(&byte3_temp))
  {
    printf("Failed receiving mouse config\n");
    return 1;
  }

  *byte1 = byte1_temp;
  *byte2 = byte2_temp;
  *byte3 = byte3_temp;

  return 0;
}

void mouse_show_config(unsigned char byte1, unsigned char byte2, unsigned char byte3)
{
  unsigned char mode, enable, scaling, leftbtn, middlebtn, rightbtn;

  mode = (byte1 & mode_mask) >> 6;
  enable = (byte1 & enable_mask) >> 5;
  scaling = (byte1 & scaling_mask) >> 4;
  leftbtn = (byte1 & leftbtn_mask) >> 2;
  middlebtn = (byte1 & middlebtn_mask) >> 1;
  rightbtn = byte1 & rightbtn_mask;

  printf("CONFIG BYTES: 0x%X,0x%X,0x%X\n\n", byte1, byte2, byte3);

  printf("Mode: %u  Enable: %u  Scaling: %u  Left Btn: %u  Middle Btn: %u  Right Btn: %u\n",
      mode, enable, scaling, leftbtn, middlebtn, rightbtn);
  printf("Resolution: %u\n",byte2);
  printf("Sample Rate: %u\n\n",byte3);
}

void mouse_print_packet()
{
  unsigned char LB, MB, RB, XOV, YOV, XSIGN, YSIGN;
  int X=0,Y=0;
  LB = (packet[0] & LB_mask);
  RB = (packet[0] & RB_mask) >> 1;
  MB = (packet[0] & MB_mask) >> 2;
  XOV = (packet[0] & XOV_mask) >> 6;
  YOV = (packet[0] & YOV_mask) >> 7;
  XSIGN = (packet[0] & XSIGN_mask) >> 4;
  YSIGN = (packet[0] & YSIGN_mask) >> 5;

  if (XSIGN)
    X = -1;

  if (YSIGN)
    Y = -1;

  // BECAUSE THE NUMBER IS IN 9 BITS IN C//2
  X = (X << 8) | packet[1];
  Y = (Y << 8) | packet[2];

  printf("B1=0x%0*X  B2=0x%0*X  B3=0x%0*X  ",2,packet[0],2,packet[1],2,packet[2]);
  printf("LB=%u MB=%u RB=%u XOV=%u YOV=%u X=%-4d Y=%-4d\n\n",LB,MB,RB,XOV,YOV,X,Y);
}

int mouse_exit_handler(){

  static state_mouse st = stateInit;

  switch (st){
  case stateInit:
    if ((packet[0] & LB_mask) == LB_mask){
      printf("Press Left Button\n\n");
      st = stateLeft;
    }
    break;

  case stateLeft:
    if ((packet[0] & ret_mask) == ret_mask){
      printf("Press Right with Left Button\n\n");
      st = stateRight;
      return 1; // SUCCESS
    }
    else
      st = stateInit;
    break;
  default:
    break;
  }
  return 0;
}
