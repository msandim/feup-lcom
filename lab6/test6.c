#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test6.h"
#include "rtc.h"


int rtc_test_conf(void) {

	unsigned long config[4];

	// get the config from reg A,B,C,D

	if (rtc_get_config(config))
		return 1;

	rtc_show_config(config);

	return 0;
}

int rtc_test_date(void) {

	unsigned long UIP, info;
	unsigned long data[7]; // array with the data info

	/* FIRST VERSION WITH UIP

  // get regA UIP
  rtc_load_info(RTC_REG_A,&UIP);

  UIP = (UIP & UIP_mask) >> 7;

  // if the flag is on, wait 244 us // !??!?!?! THIS WILL BE LIKE THIS?!?!?
  if (UIP)
  {
    printf("UIP IS ON, LETS WAIT\n");
    tickdelay(micros_to_ticks(DELAY_244));
  }

	rtc_get_data(data);

	rtc_show_data(data);

	 FIRST VERSION ENDS HERE*/

	int ipc_status;
	message msg;

	//Subscribe interrupts
	int irq_set = rtc_subscribe_int();

	//Enable Periodic interrupts in register B
	rtc_load_info(RTC_REG_B, &info);
	info ^= PIE_mask;
	rtc_save_info(RTC_REG_B, info);

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

					// clear IRQF
					rtc_load_info(RTC_REG_C, &info);
					info ^= IRQF_mask;
					rtc_save_info(RTC_REG_C, info);

					// get data
					rtc_get_data(data);
					rtc_show_data(data);

					exit_flag = 1;
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	//Disable Update ended interrupts in register B
	rtc_load_info(RTC_REG_B, &info);
	info ^= PIE_mask;
	rtc_save_info(RTC_REG_B, info);


	rtc_unsubscribe_int();
}

int rtc_test_int(unsigned long delta) {
	printf("\ntest_int: Setting alarm within %u s\n\n", delta);

	unsigned long data[7], info;

	unsigned long left, right, final_s, final_m, final_h;

	//Reads date and adds delta
	rtc_get_data(data);

	rtc_show_data(data);

	left = data[0];
	left = left >> 4;
	right = data[0]&right_mask;
	final_s = right + left * 10;

	left = data[1];
	left = left >> 4;
	right = data[1]&right_mask;
	final_m = right + left * 10;

	left = data[2];
	left = left >> 4;
	right = data[2]&right_mask;
	final_h = right + left * 10;

	final_s += delta;

	while (final_s > 59){
		final_s -= 60;
		final_m ++;
	}
	while (final_m > 59){
		final_m -= 60;
		final_h ++;
	}
	while (final_h > 23){
		final_h -= 24;
	}

	right = final_s % 10;
	left = final_s /10;

	data[0] = ((left<<4) | right);

	right = final_m % 10;
	left = final_m /10;

	data[1] = ((left<<4) | right);

	right = final_h % 10;
	left = final_h /10;

	data[2] = ((left<<4) | right);

	printf("data0 = %X\n",data[0]);
	printf("data1 = %X\n",data[1]);
	printf("data2 = %X\n",data[2]);

	rtc_show_data(data);


	rtc_set_alarm(data);


	//Enable Alarm interrupts in register B
	rtc_load_info(RTC_REG_B, &info);
	info ^= AIE_mask;
	rtc_save_info(RTC_REG_B, info);

	//Subscribe interrupts
	int ipc_status;
	message msg;

	int irq_set = rtc_subscribe_int();

	char result;

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

					result = rtc_interrupt_handler();

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	printf("result = %X", result);







	rtc_unsubscribe_int();

	//Disable Alarm interrupts in register B
	rtc_load_info(RTC_REG_B, &info);
	info ^= AIE_mask;
	rtc_save_info(RTC_REG_B, info);

}
