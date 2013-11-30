#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test5.h"
#include "mouse.h"

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

          if (mouse_ended_packet()){
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
