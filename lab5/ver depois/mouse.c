#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "mouse.h"

static int hook_id;

// MOUSE FUNCTIONS

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

// executes a cmd to IN_BUF
// returns 0 in sucess, 1 in non-success
int mouse_send_cmd(unsigned long cmd, port_t port)
{
  unsigned long stat;

  unsigned long timeoutcounter = 0;

  while(timeoutcounter < DELAY_US*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return 1;

    /* loop while 8042 input buffer is not empty */
    if( (stat & IBF) == 0 ) { // if the IBF bit is 0!

      if (sys_outb(port, cmd) != OK)
        return 1;

      // SUCCESS
      tickdelay(micros_to_ticks(DELAY_US));
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));

    timeoutcounter += DELAY_US;
  }

  return 1;
}

// receives data from OUT_BUF
// returns 0 in success, 1 in non-sucess
int mouse_receive_data_kbc(unsigned char *data)
{
  unsigned long stat, data_long;

  unsigned long timeoutcounter = 0;

  while( timeoutcounter < DELAY_US*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return -1;

    // only receive if OBF is 1 -> out_buffer is full
    if( stat & OBF ) {
      if (sys_inb(OUT_BUF, &data_long) != OK)
        return 1;

      if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
      {
        *data = (unsigned char) data_long;
        return 0;
      }

      else
        return 1;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    timeoutcounter += DELAY_US;
  }
  return 1;
}

int mouse_receive_data_kbd(unsigned char *data)
{

  unsigned long stat, data_long;

  unsigned long timeoutcounter = 0;

  while( timeoutcounter < DELAY_US*10 ) {

    if (sys_inb(OUT_BUF, &data_long) != OK)
      return 1;

    if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
    {
      *data = (unsigned char) data_long;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    timeoutcounter += DELAY_US;
  }
  return 1;
}

