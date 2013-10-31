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
}
