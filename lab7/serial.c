#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "serial.h"

int ser_get_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long* result)
{
  if (sys_inb(base_addr + relative_addr, result) != OK)
  {
    printf("serial.c: Error getting register in 0x%x",base_addr+relative_addr);
    return 1;
  }
  else
    return 0;
}

int ser_set_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long value)
{
  if (sys_outb(base_addr + relative_addr, value) != OK)
  {
    printf("serial.c: Error writting in register in 0x%x",base_addr+relative_addr);
    return 1;
  }
  else
    return 0;
}

int ser_get_bit_rate(unsigned short base_addr,unsigned long* bit_rate)
{
  // get LCR
  unsigned long lcr;

  if (ser_get_reg(base_addr,UART_LCR,&lcr))
    return 1;

  // set DLAB in LCR
  lcr |= UART_LCR_DLAB;

  // save new LCR
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // read DLL and DLM
  unsigned long dll,dlm;
  if (ser_get_reg(base_addr,UART_DLL,&dll) || ser_get_reg(base_addr,UART_DLM,&dlm))
    return 1;

  // disable DLAB and save
  lcr ^= UART_LCR_DLAB;
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // return bit_rate by parameter
  *bit_rate = BITRATE_CONSTANT/((dlm << 8) | dll);

  return 0;
}

int ser_set_bit_rate(unsigned short base_addr,unsigned long bit_rate)
{
  // get LCR
  unsigned long lcr;

  if (ser_get_reg(base_addr,UART_LCR,&lcr))
    return 1;

  // set DLAB in LCR
  lcr |= UART_LCR_DLAB;

  // save new LCR
  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  // set DLL and DLM
  unsigned long div = BITRATE_CONSTANT/bit_rate;
  unsigned long dll = div & (0x0F);
  unsigned long dlm = (div >> 8);

  if (ser_set_reg(base_addr,UART_DLL,dll) || ser_set_reg(base_addr,UART_DLM,dlm))
    return 1;

  // disable DLAB
  lcr ^= UART_LCR_DLAB;

  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

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

int ser_send_string_poll(unsigned short base_addr, char string[])
{
  unsigned int char_count=0;

  // continue sending chars till we find the null terminator
  while (string[char_count] != '\0')
  {
    ser_send_char_poll(base_addr,string[char_count]);
    char_count++;
  }

  ser_send_char_poll(base_addr,string[char_count]); // send "\0"
  return 0;

}

void ser_send_char_poll(unsigned short base_addr,unsigned char char_send)
{
  unsigned long lsr;
  ser_get_reg(base_addr,UART_LSR,&lsr);

  unsigned int wait_times = 0;

  while( !(lsr & UART_LSR_THR_EMPTY) && wait_times < 100) // repeat the cycle while THR is not empty or timeout!!!
  {
    tickdelay(micros_to_ticks(DELAY_POLL)); // lets wait till we check again
    ser_get_reg(base_addr,UART_LSR,&lsr); // get the reg again and then analyze it
    wait_times++;
  }

  // now that it's empty, we can send the char
  ser_set_reg(base_addr,UART_THR, char_send);
  printf("CHAR %c FOI ENVIADO!\n",char_send);
}

int ser_receive_string_poll(unsigned short base_addr)
{

}
