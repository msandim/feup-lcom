#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test6.h"

static int hook_id;

int rtc_test_conf(void) {

  unsigned long config[4];

  // get the config from reg A,B,C,D

  if (rtc_get_config(config))
    return 1;

  printf("A: 0x%X, B: 0x%X, C: 0x%X, D: 0x%X\n\n",
      config[0],config[1],config[2],config[3]);
}

int rtc_test_date(void) {
  /* To be completed */
}

int rtc_test_int(/* to be defined in class */) { 
  /* To be completed */
}


int rtc_subscribe_int()
{
  // set hook_id with the bit to put to one
  hook_id = RTC_IRQ;

  // create a bit_mask with the int active
  int bit_mask = BIT(hook_id);

  // use both policies
  int policy = IRQ_REENABLE | IRQ_EXCLUSIVE;

  // hook_id will be changed with a different ID, but no problem
  // because we have the bit_mask already :-)
  if (sys_irqsetpolicy(RTC_IRQ,policy,&hook_id) != OK ||
      sys_irqenable(&hook_id) != OK)
    return -1;

  return bit_mask;
}


int rtc_unsubscribe_int()
{
  if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  else
    return 0;
}

int rtc_load_data(unsigned long addr, unsigned long *info)
{
  // send the address to RTC_ADDR_REG
  if (sys_outb(RTC_ADDR_REG,addr) != OK)
    return 1;

  // receive the content of the reg
  if (sys_inb(RTC_DATA_REG,info) != OK)
    return 1;

  return 0;
}

int rtc_save_data(unsigned long addr, unsigned long info_to_send)
{
  // send the address to RTC_ADDR_REG
  if (sys_outb(RTC_ADDR_REG,addr) != OK)
    return 1;

  // send the content of the reg
  if (sys_outb(RTC_DATA_REG,info_to_send) != OK)
    return 1;

  return 0;
}

int rtc_get_config(unsigned long config[])
{
  if (rtc_load_data(RTC_REG_A,&config[0]) != OK
      || rtc_load_data(RTC_REG_B,&config[1]) != OK
      || rtc_load_data(RTC_REG_C,&config[2]) != OK
      || rtc_load_data(RTC_REG_D,&config[3]) != OK)
    return 1;
  else
    return 0;
}
