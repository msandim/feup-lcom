#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test5.h"
#include "mouse.h"

int test_packet() {
	int ipc_status;
		message msg;


		// know what ints are we interested in
		int irq_set = mouse_subscribe_int();

		int count = 0;


		while( count != 30 ) {

			/* Get a request message. */
			if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
				printf("driver_receive failed\n");
				continue;
			}

			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

						unsigned char code;

						if (mouse_receive_data_kbd(&code))
							return 1;

						count ++;

						printf("%u 0x%X\n\n", count, code);
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
