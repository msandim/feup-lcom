#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"

unsigned int intCounter = 0;
unsigned int hook_id = 0;
// STATIC ???????

int timer_set_square(unsigned long timer, unsigned long freq) {

	if (timer > 2 || freq == 0) // since it's unsigned
		return 1;

	// input_to_control is what we're changing to pass as parameter to kernel call
	unsigned char input_to_control = 0; // all at zero - 8 bits
	unsigned char timer_mask, timer_port, lsb_freq, msb_freq;
	unsigned long freq_divisor;

	// depends on the timer selected
	if (timer == 0)
	{
		timer_mask = TIMER_SEL0;
		timer_port = TIMER_0;
	}
	else if (timer == 1)
	{
		timer_mask = TIMER_SEL1;
		timer_port = TIMER_1;
	}
	else
	{
		timer_mask = TIMER_SEL2;
		timer_port = TIMER_2;
	}

	input_to_control |= timer_mask; // select timer

	input_to_control |= TIMER_LSB_MSB; // select to send lsb and msb

	input_to_control |= TIMER_SQR_WAVE; // select option 3 (square wave) in counting mode

	input_to_control |= TIMER_BIN; // count in binary

	freq_divisor = TIMER_FREQ / freq; // obtain the divisor

	// divide divisor in LSB and MSB
	lsb_freq = (char) freq_divisor; //  because this cast only preserves lsb
	msb_freq = (char) (freq_divisor >> 8); // so let's put the msb in lsb

	if (sys_outb(TIMER_CTRL, input_to_control) != OK
			|| sys_outb(timer_port, lsb_freq) != OK
			|| sys_outb(timer_port, msb_freq) != OK) // IF ONE OF THESE GIVE ERROR
		return 1;
}

int timer_subscribe_int(void ) {

	return 1;
}

int timer_unsubscribe_int() {

	sys_irqsetpolicy(???,IRQ_REENABLE,hook_id);

	return 1;
}

void timer_int_handler() {

	intCounter++;
}

int timer_get_config(unsigned long timer, unsigned char *st) {

	return 1;
}

int timer_show_config(unsigned long timer) {

	return 1;
}

int timer_test_square(unsigned long freq) {

	// test for 0
	if (timer_set_square(0,freq))
		return 0;
	else
		return 1;
}

int timer_test_int(unsigned long time) {


	// ALTERAR COM O NETO

	int ipc_status;
	message msg;
	6:
	while( 1 ) { /* You may want to use a different condition */
		/* Get a request message. */
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					...   /* process it */
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	return 1;
}

int timer_test_config(unsigned long timer) {

	return 1;
}
