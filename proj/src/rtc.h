#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

// data structures
typedef struct
{
  unsigned long seconds, minutes, hours, month, year, week_day, month_day;
} date_info;


// FUNCTIONS ****************************************************

// returns the config-data in the array config (each position -> a register from A to D)
// returns 0 in success, 1 in non-success
int rtc_get_config(unsigned long config[]);

// shows the config-data in the array config (each position -> a register from A to D)
void rtc_show_config(unsigned long config[]);

// returns the data-info in the struct "data"
// returns 0 in success, 1 in non-success
int rtc_get_data(date_info *data);

// shows the data-info in the struct "data"
void rtc_show_data(date_info data);

// 3 ways of getting the time!
void rtc_get_time_UIP(date_info *data);
void rtc_get_time_UIE_int(date_info *data);
void rtc_get_time_periodic_int(date_info *data);

// sets the alarm with the values given in the struct "data"
int rtc_set_alarm(date_info data);

// sends the address "addr" to 0x70, and reads the content of 0x71 into info
// on other words, reads the addr register.
// returns 0 in success, 1 in non-success
int rtc_load_info(unsigned long addr, unsigned long *info);

// sends the address "addr" to 0x70, and sends info to 0x71
// on other words, sets the addr register.
// returns 0 in success, 1 in non-success
int rtc_save_info(unsigned long addr, unsigned long info_to_send);

// Subscribes and enables rtc interrupts
// Returns a bit order in interrupt mask if successful, -1 if not
int rtc_subscribe_int();

// Unsubscribes mouse interrupts
// Returns 0 if success, non-zero if not
int rtc_unsubscribe_int();

// Handles RTC interrupt
// Returns 1 if a UIE interrupt occurs, 2 if a periodic interrupt occurs, 3 if an alarm int
// occurs, or 0 if none
unsigned long rtc_interrupt_handler();


#endif
