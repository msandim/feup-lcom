#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test4.h"
#include "keyboard.h"

int test_scan() {

  int ipc_status;
  message msg;
  unsigned char makebreakcode=0;

  // know what ints are we interested in
  int irq_set = keyboard_subscribe_int();

  // two_byte_scan_code verifies if the current scan code is 2 byte long
  int two_byte_scan_code = 0;

  // 0-> makecode; 1-> breakcode; 2-> E0
  int codeType;

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

          // receive
          int status = keyboard_interrupt_handler(&makebreakcode);

          // if error
          if (status == -1)
            return 1;

          // check if it's break, make or E0

          if (status == 0)
          {
            printf("Makecode: ");

            if (two_byte_scan_code == 1)
            {
              printf("0xE0 ");
              two_byte_scan_code = 0;
            }
          }
          else if (status == 1)
          {
            printf("Breakcode: ");

            if (two_byte_scan_code == 1)
            {
              printf("0xE0 ");
              two_byte_scan_code = 0;
            }
          }

          else two_byte_scan_code = 1;

          if (status != 2)
            printf("0x%X\n\n",makebreakcode);
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

  unsigned short i;
  for (i = 0; i < n; i++) {
    printf("Toggling LED number %d in array.\n",leds[i]);
    int led = leds[i];
    keyboard_toggle_led(led);
    timer_interrupt_wait(1);
  }

}
