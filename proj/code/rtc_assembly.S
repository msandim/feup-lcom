#include "rtc_constants.h"

.global _rtc_interrupt_handler
.global _clear_interrupts
.global _set_interrupts

.data
_rtc_interrupt_handler:
	movb $RTC_REG_C, %al
	outb $RTC_ADDR_REG
	inb $RTC_DATA_REG

	movb %al, %cl
  andb $UF_mask, %cl // if its an update interrupt
  jne UPDATE

	movb %al, %cl
	andb $PF_mask, %cl // if its a periodic interrupt
	jne PERIODIC

  movb %al, %cl
  andb $AF_mask, %cl // if its an alarm interrupt
  jne ALARM

  movb $0x00, %al // if its not an interrupt
  jmp END

  UPDATE:
  movl $0x01, %eax
  jmp END

  PERIODIC:
  movl $0x02, %eax
  jmp END

  ALARM:
  movl $0x03, %eax
  jmp END

	END:
	ret

_clear_interrupts:
	cli
	ret

_set_interrupts:
	sti
	ret

