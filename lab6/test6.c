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

  unsigned long UIP, info;
  unsigned long data[7]; // array with the data info

  int ipc_status;
  message msg;

  //Subscribe interrupts
  int irq_set = rtc_subscribe_int();

  // set MAX period for periodic interrupts
  rtc_load_info(RTC_REG_A, &info);
  info |= RATE_MAX_mask;
  rtc_save_info(RTC_REG_A, info);

  //Enable Periodic interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info |= PIE_mask;
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

          // read register C
          rtc_load_info(RTC_REG_C, &info);

          // read register C
          rtc_load_info(RTC_REG_C, &info);

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

  //Disable periodic interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  //info &= ~PIE_mask;
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

  printf("DATA NOW: \n");
  rtc_show_data(data);

  // transforms the h-m-s from BCD into binary
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

  // adds the delta to the seconds
  final_s += delta;

  // distributes the seconds that are too much to the minutes
  while (final_s > 59){
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
  right = final_s % 10;
  left = final_s /10;

  data[0] = ((left<<4) | right);

  right = final_m % 10;
  left = final_m /10;

  data[1] = ((left<<4) | right);

  right = final_h % 10;
  left = final_h /10;

  data[2] = ((left<<4) | right);


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
          if (result){
            exit_flag = 1;
          }

          printf("\n\ntest_int: Alarm raised after %us \n\n", delta);
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
}
