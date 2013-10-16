#include <minix/syslib.h>
#include <minix/drivers.h>

#include <minix/com.h>

#include "keyboard.h"

int keyboard_subscribe_int()
{
  // set hook_id with the bit to put to one
  hook_id = KBD_IRQ;

  // create a bit_mask with the int active
  int bit_mask = BIT(hook_id);

  // use both policies
  int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;

  // hook_id will be changed with a different ID, but no problem
  // because we have the bit_mask already :-)
  if (sys_irqsetpolicy(KBD_IRQ,policy,&hook_id) != OK ||
      sys_irqenable(&hook_id) != OK)
    return -1;

  return bit_mask;
}

int keyboard_unsubscribe_int()
{
  if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  else
    return 0;
}

char keyboard_get_code()
{

}

int keyboard_make_or_break(char code)
{
  if (code & 0x80 == 0x80)
    return 1;
  else return 0;
}

void keyboard_int_handler()
{

}
