#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "rtc.h"
#include "rtc_constants.h"

// RTC VARIABLES
static Date_info current_time;

/* RTC STATUS FUNCTIONS */
int updateRTCStatus()
{
  unsigned long status = rtc_interrupt_handler();

  if (status == 1) // if its a uie interrupt
  {
    rtc_get_data(&current_time);
    return 1;
  }

  else return 0;
}

void initRTCuieInt()
{
  unsigned long info;

  //Enable Update ended interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info |= UIE_mask;
  rtc_save_info(RTC_REG_B, info);
}

void shutRTCuieInt()
{
  unsigned long info;

  //Disable UIE interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info ^= UIE_mask;
  rtc_save_info(RTC_REG_B, info);

  // READ REG C (JUST IN CASE)
  unsigned long test;
  rtc_load_info(RTC_REG_C, &test);
}

Date_info getRTCtime()
{
  return current_time;
}
