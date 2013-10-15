

#include <minix/sysutil.h>

#include "test4.h"

static int hook_id;

int test_scan(void) {

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

          keyboard_int_handler();
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

}

int test_leds(unsigned short n, unsigned short *leds) {
  /* To be completed */
}

// real functions


