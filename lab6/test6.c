#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test6.h"
#include "rtc.h"
#include "rtc_constants.h"


int rtc_test_conf(void) {

  unsigned long config[4];

  // get the config from reg A,B,C,D

  if (rtc_get_config(config))
    return 1;

  rtc_show_config(config);

  return 0;
}

int rtc_test_date(void) {

  unsigned long data[7]; // array with the data info

  //rtc_get_time_UIP(data);
  //rtc_get_time_UIE_int(data);
  rtc_get_time_periodic_int(data);

  rtc_show_data(data);

  return 0;

}

int rtc_test_int(unsigned long delta) {
  printf("\ntest_int: Setting alarm within %u s\n\n", delta);

  unsigned long data[7], info;

  unsigned long final_s, final_m, final_h;

  //Reads date and adds delta -> data has time in this format: seconds, minutes, hours
  // weekday, monthday,month,year
  rtc_get_data(data);

  printf("DATA NOW: \n");
  rtc_show_data(data);

  // transforms the h-m-s from BCD into binary
  // seconds
  final_s = binary_to_BCD(data[0]);
  // minutes
  final_m = binary_to_BCD(data[1]);
  // hours
  final_h = binary_to_BCD(data[2]);

  // adds the delta to the seconds
  final_s += delta;

  // distributes the seconds that are too much to the minutes
  while (final_s > 59) {
    final_s -= 60;
    final_m ++;
  }

  // and distributes the minutes that are too much to the hours
  while (final_m > 59){
    final_m -= 60;
    final_h ++;
  }
  while (final_h > 23){
    final_h -= 24;
  }

  // now .. transform the binary in BCD again and save in "data"
  data[0] = BCD_to_binary(final_s);
  data[1] = BCD_to_binary(final_m);
  data[2] = BCD_to_binary(final_h);

  printf("\nTHE ALARM WILL BE SET ON AT:\n");
  rtc_show_data(data);
  rtc_set_alarm(data);

  //Subscribe interrupts
  int ipc_status;
  message msg;

  int irq_set = rtc_subscribe_int();

  //Enable Alarm interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info |= AIE_mask;
  rtc_save_info(RTC_REG_B, info);

  unsigned long status;

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

          status = rtc_interrupt_handler();
          if (status == 3){ // if its an alarm interrupt
            exit_flag = 1;
            printf("\n\ntest_int: Alarm raised after %us \n\n", delta);
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

  //Disable Alarm interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info ^= AIE_mask;
  rtc_save_info(RTC_REG_B, info);

  rtc_unsubscribe_int();

  return 0;
}
