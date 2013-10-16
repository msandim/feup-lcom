#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test4.h"

static int hook_id;

int test_scan() {

  int ipc_status;
  message msg;

  // know what ints are we interested in
  int irq_set = keyboard_subscribe_int();

  unsigned char makebreakcode = 0;

  // while esc isnt pressed (and released)
  while( makebreakcode != 0x81 ) {

    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          keyboard_send_kbc_cmd(0xF6);

          keyboard_receive_data_kbc(&makebreakcode);

          printf("CODE: 0x%X\n\n",makebreakcode);
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
  keyboard_unsubscribe_int();

  return 0;
}

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
int keyboard_send_kbc_cmd(unsigned long cmd)
{

  unsigned long stat;

  unsigned long timeoutcounter = 0;

  while(timeoutcounter < WAIT_KBC*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return 1;

    /* loop while 8042 input buffer is not empty */
    if( (stat & IBF) == 0 ) { // if the IBF bit is 0!

      if (sys_outb(KBC_CMD_REG, cmd) != OK)
        return 1;

      // SUCCESS
      return 0;
    }
    //delay(WAIT_KBC); // if the IBF bit is 1! IS THIS IN MILI SECONDS?
    sleep(2);

    timeoutcounter += WAIT_KBC;
  }

  return 1;
}

// receives data from OUT_BUF
// returns 0 in success, 1 in non-sucess

int keyboard_receive_data_kbc(unsigned char *data)
{

  unsigned long stat, data_long;

  unsigned long timeoutcounter = 0;

  while( timeoutcounter < WAIT_KBC*10 ) {

    if (sys_inb(STAT_REG, &stat) != OK)
      return -1;

    /* loop while 8042 output buffer is empty */
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
    //delay(WAIT_KBC);
    sleep(2);

    timeoutcounter += WAIT_KBC;

  }

  return 1;

}

int keyboard_make_or_break(char code)
{
  if (code & 0x80 == 0x80)
    return 1;
  else return 0;
}


int test_leds(unsigned short n, unsigned short *leds) {
  /* To be completed */
}


