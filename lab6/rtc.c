#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "rtc.h"
#include "rtc_constants.h"

// Funcoes de Assembly:
unsigned long rtc_interrupt_handler();
void clear_interrupts();
void set_interrupts();

static int hook_id;

int rtc_get_config(unsigned long config[])
{
  if (rtc_load_info(RTC_REG_A,&config[0]) != OK
      || rtc_load_info(RTC_REG_B,&config[1]) != OK
      || rtc_load_info(RTC_REG_C,&config[2]) != OK
      || rtc_load_info(RTC_REG_D,&config[3]) != OK)
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

  printf("A: 0x%X, B: 0x%X, C: 0x%X, D: 0x%X\n\n",
      config[0],config[1],config[2],config[3]);

  printf("Register A:\n");
  printf("UIP: %X | Rate: %u\n\n",UIP,rate);

  printf("Register B:\n");
  printf("SET: %X | PIE: %X | AIE: %X | UIE: %X | SQWE: %X | DM: %X | 12 hour system? (12h-1;24h-0): %X | DSE: %X\n\n",
      SET, PIE, AIE, UIE, SQWE, DM, type_hour, DSE);

  printf("Register C\n");
  printf("IRQF: %X | PF: %X | AF: %X | UF: %X\n\n",
      IRQF, PF, AF, UF);

  printf("Register D\n");
  printf("VRT: %X\n",VRT);
}

int rtc_get_data(unsigned long data[])
{
  // read data
  rtc_load_info(RTC_REG_SECONDS,&data[0]);
  rtc_load_info(RTC_REG_MINUTES,&data[1]);
  rtc_load_info(RTC_REG_HOURS,&data[2]);
  rtc_load_info(RTC_REG_WEEK_DAY,&data[3]);
  rtc_load_info(RTC_REG_MONTH_DAY,&data[4]);
  rtc_load_info(RTC_REG_MONTH,&data[5]);
  rtc_load_info(RTC_REG_YEAR,&data[6]);

  return 0;
}

void rtc_show_data(unsigned long data[])
{
  printf("The time:\n");
  printf("%x:%x:%x - %x/%x/%x\n",data[2],data[1],data[0],data[4],data[5],data[6]);
  printf("Week day: %x\n",data[3]);
}

void rtc_get_time_UIP(unsigned long data[])
{
  unsigned long UIP;

  // get regA UIP
  rtc_load_info(RTC_REG_A,&UIP);

  UIP = (UIP & UIP_mask) >> 7;

  // if the flag is on, wait 244 us
  if (UIP)
  {
    //printf("UIP IS ON, LETS WAIT\n");
    tickdelay(micros_to_ticks(DELAY_244));
  }

  rtc_get_data(data);
}

void rtc_get_time_UIE_int(unsigned long data[])
{
  unsigned long status, info;

  int ipc_status;
  message msg;

  //Subscribe interrupts
  int irq_set = rtc_subscribe_int();

  //Enable Update ended interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info |= UIE_mask;
  rtc_save_info(RTC_REG_B, info);

  int exit_flag = 0;

  while( exit_flag == 0 ) {

    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          status = rtc_interrupt_handler();
          if (status == 1) // if its a UIE interrupt!
          {
            // get data
            // data has time in this format: seconds, minutes, hours
            // weekday, monthday,month,year
            rtc_get_data(data);
            exit_flag = 1;
          }
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  //Disable UIE interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info ^= UIE_mask;
  rtc_save_info(RTC_REG_B, info);

  rtc_unsubscribe_int();
}

void rtc_get_time_periodic_int(unsigned long data[])
{
  unsigned long status,info,reg_A_backup;

  int ipc_status;
  message msg;

  //Subscribe interrupts
  int irq_set = rtc_subscribe_int();

  // set MAX period for periodic interrupts
  rtc_load_info(RTC_REG_A, &info);
  reg_A_backup = info; // BACKUP OF REG A
  info |= RATE_MAX_mask;
  rtc_save_info(RTC_REG_A, info);

  //Enable Periodic interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  info |= PIE_mask;
  rtc_save_info(RTC_REG_B, info);

  int exit_flag = 0;

  while( exit_flag == 0 ) {

    /* Get a request message. */
    if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

          status = rtc_interrupt_handler();
          if (status == 2) // if its a periodic interrupt
          {
            // get data
            // data has time in this format: seconds, minutes, hours
            // weekday, monthday,month,year
            rtc_get_data(data);
            exit_flag = 1;
          }

        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }

  //Disable periodic interrupts in register B
  rtc_load_info(RTC_REG_B, &info);
  //info &= ~PIE_mask;
  info ^= PIE_mask;
  rtc_save_info(RTC_REG_B, info);

  // backup of reg A because of rate
  rtc_save_info(reg_A_backup, info);

  rtc_unsubscribe_int();
}


int rtc_set_alarm(unsigned long data[])
{
  // read data
  rtc_save_info(RTC_REG_SECONDS_ALARM,data[0]);
  rtc_save_info(RTC_REG_MINUTES_ALARM,data[1]);
  rtc_save_info(RTC_REG_HOURS_ALARM,data[2]);

  return 0;
}

int rtc_load_info(unsigned long addr, unsigned long *info)
{
  // send the address to RTC_ADDR_REG
  if (sys_outb(RTC_ADDR_REG,addr) != OK)
    return 1;

  // receive the content of the reg
  if (sys_inb(RTC_DATA_REG,info) != OK)
    return 1;

  return 0;
}

int rtc_save_info(unsigned long addr, unsigned long info_to_send)
{
  // send the address to RTC_ADDR_REG
  if (sys_outb(RTC_ADDR_REG,addr) != OK)
    return 1;

  // send the content of the reg
  if (sys_outb(RTC_DATA_REG,info_to_send) != OK)
    return 1;

  return 0;
}

unsigned long binary_to_BCD(unsigned long valuebin)
{
  unsigned long left = valuebin >> 4;
  unsigned long right = valuebin&right_mask;

  return right + left * 10;
}

unsigned long BCD_to_binary(unsigned long valuebcd)
{
  unsigned long right = valuebcd % 10;
  unsigned long left = valuebcd /10;

  return ((left<<4) | right);

  return right + left * 10;
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
