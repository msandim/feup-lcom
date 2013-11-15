#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "serial.h"

int ser_get_lcr(unsigned short base_addr,unsigned long* lcr)
{
  if (sys_inb(base_addr + UART_LCR, lcr) != OK)
    return 1;

  else
    return 0;
}

int ser_get_ier(unsigned short base_addr,unsigned long* ier)
{
  if (sys_inb(base_addr + UART_IER, ier) != OK)
    return 1;

  else
    return 0;
}

int ser_get_bit_rate(unsigned short base_addr,unsigned long* bit_rate)
{
  // get LCR
  unsigned long lcr;

  if (ser_get_lcr(base_addr,&lcr))
    return 1;

  // set DLAB in LCR
  lcr ^= UART_LCR_DLAB;

  // save new LCR
  if (sys_outb(base_addr + UART_LCR, &lcr) != OK)
    return 1;

  // read DLL and DLM
  unsigned long dll,dlm;
  if (sys_inb(base_addr + UART_DLL,&dll) != OK || sys_inb(base_addr + UART_DLM,&dlm) != OK)
    return 1;

  // disable DLAB and save
  lcr ^= UART_LCR_DLAB;
  if (sys_outb(base_addr + UART_LCR, lcr) != OK)
    return 1;

  // return bit_rate by parameter
  *bit_rate = BITRATE_CONSTANT/((dlm << 8) | dll);

  return 0;
}

void ser_show_lcr(unsigned long lcr)
{
  printf("LCR REGISTER: 0x%x\n\n", lcr);

  printf("Number of bits per char: ");

  if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_5)
    printf("5\n");
  else if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_6)
    printf("6\n");
  else if ((lcr & UART_LCR_WORD_LENGTH) == UART_LCR_WORD_LENGTH_7)
    printf("7\n");
  else
    printf("8\n");

  printf("Number of stop bits: ");

  if ((lcr & UART_LCR_STOP_BIT) == UART_LCR_STOP_BIT_1)
    printf("1\n");
  else
    printf("2\n");

  printf("Parity Control: ");

  if ((lcr & UART_LCR_PARITY_BIT_3) == 0x0) // if parity is "X X 0"
    printf("no parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_ODD_PARITY)
    printf("odd parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_EVEN_PARITY)
    printf("even parity\n");
  else if ((lcr & UART_LCR_PARITY) == UART_LCR_1_PARITY)
    printf("always 1\n");
  else
    printf("always 0\n");

  printf("DLAB: ");

  if ((lcr & UART_LCR_DLAB) == UART_LCR_DLAB_DATA)
    printf("0 (Acess Registers RBR e THR)\n");
  else
    printf("1 (Divisor Latch Access)\n");

  printf("\n\n");
}

void ser_show_ier(unsigned long ier)
{
  printf("IER REGISTER: 0x%x\n\n",ier);

  printf("Received Data Interrupts: ");

  if (ier & UART_IER_RD_INT)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Received Transmitter Holding Register Empty Interrupt: ");

  if (ier & UART_IER_TE_INT)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Receiver Line Status Interrupt: ");

  if (ier & UART_IER_RLS_INT)
    printf("ON\n");
  else
    printf("OFF\n");
}


