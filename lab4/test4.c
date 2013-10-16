#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test4.h"
#include "keyboard.h"

int test_scan() {

  int ipc_status;
  message msg;

  // know what ints are we interested in
  int irq_set = keyboard_subscribe_int();

  // enable KBD and set default values
  keyboard_send_kbc_cmd(enableKBDdefault, KBC_CMD_REG);

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

          keyboard_receive_data_kbc(&makebreakcode);

          if (keyboard_make_or_break(makebreakcode) == 0)
            printf("\nMakecode: ");
          else
            printf("\nBreakcode: ");

          printf("0x%X\n",makebreakcode);
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

int test_leds(unsigned short n, unsigned short *leds) {
  /* To be completed */
}
