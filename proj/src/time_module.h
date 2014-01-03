#ifndef _TIME_MODULE_H_
#define _TIME_MODULE_H_

/** @defgroup time_module Time Module
 * @{
 *
 * Higher level module that handles with RTC interrupts and updates its status
 */

#include "rtc.h"

/**
 * @brief Updates the RTC status in the program
 *
 * This function is the top layer of the RTC module.
 * It's called everytime a RTC interrupt occurs, and using the interrupt handler to get
 * the date info, fills the static struct "current_time"
 *
 *
 * @return 1 if a UIE Interrupt occured and time info is fetched, 0 if not
 */
int updateRTCStatus();

/**
 * @brief Initiates UIE Interrupts in RTC register B
 *
 * This function is called in runDevices() to initiate RTC event activity
 *
 */
void initRTCuieInt();

/**
 * @brief Shuts down UIE Interrupts in RTC register B
 *
 * This function is called in runDevices() to shut down RTC event activity
 *
 */
void shutRTCuieInt();

/**
 * @brief Returns the "current_time" struct with the updated time
 *
 * @return current_time struct
 */
Date_info getRTCtime();

/** @} */

#endif
