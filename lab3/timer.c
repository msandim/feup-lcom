#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "i8254.h"

unsigned int intCounter = 0;
int hook_id;
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
	else if (timer == 2)
	{
		timer_mask = TIMER_SEL2;
		timer_port = TIMER_2;
	}

	else
	{
		printf("\nTIME_SET_SQUARE: INVALID TIMER\n");
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

int timer_subscribe_int() {

	// set hoo_id with the bit to put to one
	hook_id = TIMER0_IRQ;

	// create a bit_mask with the int active
	int bit_mask = BIT(hook_id);

	// hook_id will be changed with a different ID, but no problem
	// because we have the bit_mask already :-)
	if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) != OK ||
	sys_irqenable(&hook_id) != OK)
		return -1;

	return bit_mask;
}

int timer_unsubscribe_int() {

	if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
		return 1;
	else
		return 0;
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

	int irq_set = timer_subscribe_int();

	printf("Time = %u\n",time);
	int ipc_status;
	message msg;

	unsigned int i= 0;
	while( /*intCounter*/ i < time*60 ) {
		/* Get a request message. */

		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed\n");
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					//timer_int_handler();
					i++;
					printf("Interruption number: %u\n",/*intCounter*/ i);
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
	return 1;
}

int timer_test_config(unsigned long timer) {

	return 1;
}
