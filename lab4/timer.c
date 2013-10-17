#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "i8254.h"

unsigned int intCounter = 0;
static int hook_id;

int timer_set_square(unsigned long timer, unsigned long freq) {

	if (timer > 2 || freq < 19) // if freq < 19, the divisor cant be put in 16 bits (overflow)
	{
		printf("Error on set_square::timer.c\n");
		return 1;
	}

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


	unsigned char timer_config;

	timer_get_config(timer,&timer_config);

	unsigned char bcd = timer_config & 0x1;

	if (bcd == 0)
		input_to_control |= TIMER_BIN; // count in binary
	else
		input_to_control |= TIMER_BCD; // count in BCD

	freq_divisor = TIMER_FREQ / freq; // obtain the divisor

	// divide divisor in LSB and MSB
	lsb_freq = (char) freq_divisor; //  because this cast only preserves lsb
	msb_freq = (char) (freq_divisor >> 8); // so let's put the msb in lsb

	if (sys_outb(TIMER_CTRL, input_to_control) != OK
			|| sys_outb(timer_port, lsb_freq) != OK
			|| sys_outb(timer_port, msb_freq) != OK) // IF ONE OF THESE GIVE ERROR
		return 1;

	return 0;
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

int timer_get_config(unsigned long timer, unsigned char* st) {

	unsigned char timer_port, timer_sel, input_mask = 0;

	if (timer > 2){
		printf("Error on get_config\n");
		return 1;
	}

	// select port
	if (timer == 0)
	{
		timer_port = TIMER_0;
	}
	else if (timer == 1)
	{
		timer_port = TIMER_1;
	}
	else
	{
		timer_port = TIMER_2;
	}

	// select to readback
	input_mask |= BIT(7);
	input_mask |= BIT(6);

	// select the timer to read
	timer_sel = TIMER_RB_SEL(timer);
	input_mask |= timer_sel;

	input_mask |= TIMER_RB_COUNT_; // read status

	if (sys_outb(TIMER_CTRL, input_mask) != OK
			|| sys_inb(timer_port,(unsigned long * ) st) != OK)
		return 1;

	return 0;
}


int timer_show_config(unsigned long timer) {

	unsigned char config_output;
	unsigned char* st = &config_output;
	if(timer_get_config (timer,st)!=0){
		printf("Error showing config::time.c\n");
		return 1;
	}

	unsigned char output, nullCount, typeAcess, operatingMode, BCD;

	// get info
	output = *st >> 7;
	nullCount = (*st >> 6) & 0x1;
	typeAcess = (*st >> 4) & 0x3;
	operatingMode = (*st >> 1) & 0x7;
	BCD = *st & 0x1;

	printf("Status from timer %lu: 0x%x\n\n",timer,*st);
	printf("Output: 0x%x\n",output);
	printf("Null Count: 0x%x\n",nullCount);

	if (typeAcess == 1)
		printf("Type Access: LSB\n");
	else if (typeAcess == 2)
		printf("Type Access: MSB\n");
	else
		printf("Type Access: LSB + MSB\n");

	if (operatingMode == 0)
		printf("Operating Mode: 0\n");
	else if(operatingMode == 1)
		printf("Operating Mode: 1\n");
	else if(operatingMode == 2 || operatingMode == 6)
		printf("Operating Mode: 2\n");
	else if(operatingMode == 3 || operatingMode == 7)
		printf("Operating Mode: 3\n");
	else if(operatingMode == 4)
		printf("Operating Mode: 4\n");
	else
		printf("Operating Mode: 5\n");

	if (BCD == 0)
		printf("Counting Mode: Binary (16 bits)\n\n");
	else
		printf("Counting Mode: BCD (4 digits with 4 bits each)\n\n");

	return 0;
}

int timer_interrupt_wait(unsigned long time){
	int irq_set = timer_subscribe_int();

	int ipc_status;
	message msg;

	unsigned int nseconds = 0;

	while( nseconds < time ) {
		/* Get a request message. */

		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed\n");
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (intCounter % 60 == 0)
					{
						nseconds ++;
					}

					timer_int_handler(); // incs counter
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
	return 0;
}

int timer_test_square(unsigned long freq) {

	// test for 0
	return timer_set_square(0,freq);

}

int timer_test_int(unsigned long time) {

	int irq_set = timer_subscribe_int();

	printf("Time = %u\n",time);
	int ipc_status;
	message msg;

	unsigned int nseconds = 0;

	while( intCounter <= time*60 ) {
		/* Get a request message. */

		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed\n");
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (intCounter % 60 == 0)
					{
						printf("Second number: %u\n",nseconds++);
					}

					timer_int_handler(); // incs counter
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
	return 0;
}

int timer_test_config(unsigned long timer) {

	return timer_show_config(timer);
}
