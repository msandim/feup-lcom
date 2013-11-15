#include "serial.h"

unsigned char ser_get_lcr(unsigned char base_addr)
{
  unsigned long lcr;
  sys_inb(base_addr + UART_LCR, &lcr);

  return (unsigned char) lcr;
}

unsigned char ser_get_ier(unsigned char base_addr)
{
  unsigned long ier;
  sys_inb(base_addr + UART_IER, &ier);

  return (unsigned char) ier;
}

void ser_show_lcr(lcr_config)
{
  cout << ....
}

void ser_show_ier(ier_config)
{
  cout << ....
}


