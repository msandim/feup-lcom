#ifndef _TIME_MODULE_H_
#define _TIME_MODULE_H_

#include "rtc.h"

// Updates the date
int updateRTCStatus();

// Initiates UIE interrupts in RTC to update time
void initRTCuieInt();

// Shuts down UIE interrupts in RTC
void shutRTCuieInt();

// Returns the struct with the current date info
date_info getRTCtime();

#endif
