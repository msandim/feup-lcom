#include "rtc.h"

/* char rtc_interrupt_handler(); */

_rtc_interrupt_handler:

movb $RTC_REG_B, %al
outb $RTC_ADDR_REG
inb $RTC_DATA_REG

ret

_clear_interrupts:
cli
ret

_set_interrupts:
sti
ret
