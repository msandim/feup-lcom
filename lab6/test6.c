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

  rtc_show_config(config);
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

void rtc_show_config(unsigned long config[])
{
  unsigned char UIP, rate, SET, PIE, AIE, UIE, SQWE, DM, type_hour, DSE, IRQF, PF, AF, UF, VRT;

  // regA
  UIP = (config[0] & UIP_mask) >> 7;
  rate = config[0] & RATE_mask;

  // regB
  SET = (config[1] & SET_mask) >> 7;
  PIE = (config[1] & PIE_mask) >> 6;
  AIE = (config[1] & AIE_mask) >> 5;
  UIE = (config[1] & UIE_mask) >> 4;
  SQWE = (config[1] & SQWE_mask) >> 3;
  DM = (config[1] & DM_mask) >> 2;
  type_hour = (config[1] & HOUR_12_24_mask) >> 1;
  DSE = (config[1] & DSE_mask);

  // regC
  IRQF = (config[2] & IRQF_mask) >> 7;
  PF = (config[2] & PF_mask) >> 6;
  AF = (config[2] & AF_mask) >> 5;
  UF = (config[2] & UF_mask) >> 4;

  // regD
  VRT = (config[3] & VRT_mask) >> 7;

  printf("A: 0x%X, B: 0x%X, C: 0x%X, D: 0x%X\n",
      config[0],config[1],config[2],config[3]);

  printf("Register A\n");
  printf("UIP: %X\nRate: %u\n\n",UIP,rate);

  printf("Register B\n");
  printf("SET: %X\nPIE: %X\nAIE: %X\nUIE: %X\nSQWE: %X\nDM: %X\n24/12: %X\nDSE: %X\n\n",
      SET, PIE, AIE, UIE, SQWE, DM, type_hour, DSE);

  printf("Register C\n");
  printf("IRQF: %X\nPF: %X\nAF: %X\nUF: %X\n\n",
      IRQF, PF, AF, UF);

  printf("Register D\n");
  printf("VRT: %X\n\n",VRT);
}
