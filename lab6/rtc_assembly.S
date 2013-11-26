#include "rtc_constants.h"

.global _rtc_interrupt_handler
.global _clear_interrupts
.global _set_interrupts

.data
_rtc_interrupt_handler:
	movb $0xC, %al
	outb $0x70
	inb $0x71
	andb $0x20, %al
	ret

_clear_interrupts:
	cli
	ret

_set_interrupts:
	sti
	ret

