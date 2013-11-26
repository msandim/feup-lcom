#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

// FUNCTIONS ****************************************************

// returns the config-data in the array config (each position -> a register from A to D)
// returns 0 in success, 1 in non-success
int rtc_get_config(unsigned long config[]);

// shows the config-data in the array config (each position -> a register from A to D)
void rtc_show_config(unsigned long config[]);

// returns the data-info in the array "data" acording to the register order (SECONDS, MINUTES, HOURS)
// returns 0 in success, 1 in non-success
int rtc_get_data(unsigned long data[]);

// shows the data-info in the array "data" acording to the register order
void rtc_show_data(unsigned long data[]);

// sets the alarm with the values given in the array "data", acordint to the register
// order
int rtc_set_alarm(unsigned long data[]);

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


#endif
