#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test5.h"

static int hook_id;

unsigned char packet[3];

unsigned short count=0;

// ********************************************
// TEST FUNCTIONS
// ********************************************
int test_packet() {
  int ipc_status;
  message msg;


  // know what ints are we interested in
  int irq_set = mouse_subscribe_int();

  int counter = 0;

  mouse_send_cmd(ENABLE_PACKETS);

  while( counter != 30 ) {

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
          counter++;
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

  return 0;

}

int test_asynch(unsigned short duration) {
  /* To be completed ... */
}
int test_config(void) {
  /* To be completed ... */
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

// executes a cmd (byte) to the mouse
// returns 0 in sucess, 1 in non-success
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

// receives data from OUT_BUF
// returns 0 in success, 1 in non-sucess
int mouse_receive_data_outbuf(unsigned char *data)
{
  unsigned long stat, data_long;

  unsigned long timeoutcounter = 0;

  while( timeoutcounter < DELAY_US*20 ) {

    // if (sys_inb(STAT_REG, &stat) != OK)
    // return 1;

    // only receive if OBF is 1 -> out_buffer is full
    //if( stat & OBF ) {
    if (sys_inb(OUT_BUF, &data_long) != OK)
      return 1;

    *data = (unsigned char) data_long;
    return 0;
    // }

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
    printf("Didnt receive data from mouse\n\n");
    return;
  }

  printf("COUNT: 0x%X\n\n",code);

  if ((count == 0 && (code & bit3_mask !=0)) || count == 1 || count == 2) // if bit 1 is on and count = 0
  { // or we're in the second or third bit
    packet[count] = code;
    count++;
  }
  else if (count == 3)
  {
    count = 0; // initializes
    mouse_print_packet(); // prints packet
  }

  return;

}

void mouse_print_packet()
{
  printf("B1=0x%X B2=0x%X B3=0x%X\n",4,packet[0],4,packet[1],4,packet[2]);
  //printf("a");
}
