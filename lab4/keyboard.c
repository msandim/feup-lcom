#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "keyboard.h"

static int hook_id;

static char led_bitmask = 0; // this will turn off every led, but cant ready current state

// KEYBOARD FUNCTIONS

int keyboard_subscribe_int()
{
  // set hook_id with the bit to put to one
  hook_id = KBD_IRQ;

  // create a bit_mask with the int active
  int bit_mask = BIT(hook_id);

  // use both policies
  int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;

  // hook_id will be changed with a different ID, but no problem
  // because we have the bit_mask already :-)
  if (sys_irqsetpolicy(KBD_IRQ,policy,&hook_id) != OK ||
      sys_irqenable(&hook_id) != OK)
    return -1;

  return bit_mask;
}

int keyboard_unsubscribe_int()
{
  if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  else
    return 0;
}

// executes a cmd to IN_BUF
// returns 0 in sucess, 1 in non-success
int keyboard_send_cmd(unsigned long cmd, port_t port)
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
int keyboard_receive_data_kbc(unsigned char *data)
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

int keyboard_receive_data_kbd(unsigned char *data)
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


int keyboard_make_or_break(unsigned char code)
{
  if (code == 0xE0)
    return 2;
  else if ((code & 0x80) == 0x80)
    return 1;
  else
    return 0;
}

int keyboard_toggle_led(unsigned short led){

  // puts on that led
  led_bitmask ^= BIT(led);

  int repeat_cmd = 1, repeat_arg = 0;

  unsigned char info;

  do
  {

    // send the command to kbd port (input)
    keyboard_send_cmd(TOGGLE_LEDS, KBD_IN_BUF);

    // error entering a command -> must repeat the command
    if (!keyboard_receive_data_kbd(&info) && info != RESEND && info != ERROR)
    {
      do
      {
        //printf("COMMAND WAS OK\n\n");
        repeat_cmd = 0;

        // sends the arg to kbd port (input)
        if (keyboard_send_cmd(led_bitmask,KBD_IN_BUF))
          return 1;

        if (keyboard_receive_data_kbd(&info))
          return 1;

        if (info == RESEND)
        {
          //printf("ERROR RESEND IN ARG!!!\n\n");
          repeat_arg = 1;
        }

        else if (info == ERROR)
        {
          //printf("ERROR ERROR IN ARG\n\n");
          repeat_arg = 0;
          repeat_cmd = 1;
        }

        else
        {
          //printf("ARG WENT COOL1\n\n");
          repeat_arg = 0;
          repeat_cmd = 0;
        }
      } while (repeat_arg);
    }

  } while (repeat_cmd);

  return 0;
}
