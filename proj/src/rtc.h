#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

/** @defgroup rtc RTC
 * @{
 *
 * Low level module that has several functions that work directly with the Real Time Clock (RTC)
 */

// Data Structures

/** @name Date Info Struct*/
/**@{
 *
 * Struct that keeps info regarding the current time and date
 */
typedef struct
{
  unsigned long seconds; /**< @brief Second number */
  unsigned long minutes; /**< @brief Minute number */
  unsigned long hours; /**< @brief Hour number */
  unsigned long month; /**< @brief Month number */
  unsigned long year; /**< @brief Year number */
  unsigned long week_day; /**< @brief Week day number */
  unsigned long month_day; /**< @brief Day of the Month number */
} date_info;

/** @} end of Date Info Struct */

// FUNCTIONS ****************************************************

/**
 * @brief Fills an array passed as parameter with the RTC register's value (each position, a reg from A to D)
 * @param config array to be filled with config info
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_get_config(unsigned long config[]);

/**
 * @brief Displays info about RTC configuration from an array passed as parameter with the RTC register's value (each position, a reg from A to D)
 * @param config array with the config info
 *
 * @return 0 on success, non-zero otherwise
 */
void rtc_show_config(unsigned long config[]);

/**
 * @brief Fills the date_info struct passed as parameter with date info
 * @param data struct to be filled with the date info
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_get_data(date_info *data);

/**
 * @brief shows the date information in the struct passed as parameter in a human friendly way
 * @param data struct with the date info
 */
void rtc_show_data(date_info data);

// 3 ways of getting the time!

/**
 * @brief Fills the date_info struct passed as parameter with date info using UIP flag method
 * @param data struct to be filled with the date info
 *
 * @return 0 on success, non-zero otherwise
 */
void rtc_get_time_UIP(date_info *data);

/**
 * @brief Fills the date_info struct passed as parameter with date info using UIE Interrupt method
 * @param data struct to be filled with the date info
 *
 * @return 0 on success, non-zero otherwise
 */
void rtc_get_time_UIE_int(date_info *data);

/**
 * @brief Fills the date_info struct passed as parameter with date info using Periodic Interrupt method
 * @param data struct to be filled with the date info
 *
 * @return 0 on success, non-zero otherwise
 */
void rtc_get_time_periodic_int(date_info *data);

/**
 * @brief Sets the alarm with the values given in the struct "data"
 * @param data struct with time info
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_set_alarm(date_info data);

/**
 * @brief Loads the contents of the register in the "addr" address to the "info" pointed variable
 * @param addr address of the register to read
 * @param info pointer to the variable to write with the register's contents
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_load_info(unsigned long addr, unsigned long *info);

/**
 * @brief Saves the contents of the parameter "info_to_send" in the register with address "addr"
 * @param addr address of the register to write
 * @param info_to_send information to write on the register
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_save_info(unsigned long addr, unsigned long info_to_send);

/**
 * @brief Subscribes and enables RTC interrupts
 *
 * Returns a bit mask with with bit 1 selected on the interrupt type.
 *
 * @return the bit mask, or -1 if it's not successful
 */
int rtc_subscribe_int();

/**
 * @brief Unsubscribes RTC interrupts
 *
 * @return 0 on success, non-zero otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief RTC Interrupt Handler (written in assembly)
 *
 * @return 1 if a UIE interrupt occurs, 2 if a periodic interrupt occurs, 3 if an alarm interrupt
 * occurs, or 0 if none
 */
unsigned long rtc_interrupt_handler();

/** @} */

#endif
