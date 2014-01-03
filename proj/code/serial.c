#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "serial.h"

static int hook_id;

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
  unsigned long dll = div & (0x00FF);
  unsigned long dlm = (div >> 8);

  if (ser_set_reg(base_addr,UART_DLL,dll) || ser_set_reg(base_addr,UART_DLM,dlm))
    return 1;

  // disable DLAB
  lcr ^= UART_LCR_DLAB;

  if (ser_set_reg(base_addr,UART_LCR,lcr))
    return 1;

  return 0;
}

int ser_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop,
    long parity, unsigned long rate)
{
  unsigned long lcr_config;

  if(ser_get_reg(base_addr,UART_LCR,&lcr_config))
  {
    printf("ser_test_set: Error reading LCR\n\n");
    return 1;
  }

  unsigned long new_lcr = 0;

  new_lcr |= (lcr_config & UART_LCR_SET_BREAK);
  new_lcr |= (lcr_config & UART_LCR_DLAB);

  //BITS
  if (bits == 5) {
    new_lcr |= UART_LCR_WORD_LENGTH_5;
  } else if (bits == 6) {
    new_lcr |= UART_LCR_WORD_LENGTH_6;
  } else if (bits == 7) {
    new_lcr |= UART_LCR_WORD_LENGTH_7;
  } else if (bits == 8) {
    new_lcr |= UART_LCR_WORD_LENGTH_8;
  } else {
    printf ("ser_test_set: Invalid number of bits %x\n", bits);
    return 1;
  }

  //STOP
  if (stop == 1) {
    new_lcr |= UART_LCR_STOP_BIT_1;
  } else if (stop == 2) {
    new_lcr |= UART_LCR_STOP_BIT_2;
  } else {
    printf ("ser_test_set: Invalid number of stop bits %x\n", stop);
    return 1;
  }

  //PARITY
  if (parity == -1) {
    new_lcr |= UART_LCR_NO_PARITY;
  } else if (parity == 0) {
    new_lcr |= UART_LCR_EVEN_PARITY;
  } else if (parity == 1) {
    new_lcr |= UART_LCR_ODD_PARITY;
  } else {
    printf ("ser_test_set: Invalid parity %x\n", parity);
    return 1;
  }

  // save new lcr
  if (ser_set_reg(base_addr,UART_LCR,new_lcr))
    return 1;

  if (ser_set_bit_rate(base_addr, rate))
    return 1;

  //printf("BASE_ADDR: %x, BITS: %x,STOP: %x,PARITY: %d, RATE: %u",base_addr,bits,stop,parity,rate);

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

  if (ier & UART_IER_ENABLE_RD)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Received Transmitter Holding Register Empty Interrupt: ");

  if (ier & UART_IER_ENABLE_TE)
    printf("ON\n");
  else
    printf("OFF\n");

  printf("Receiver Line Status Interrupt: ");

  if (ier & UART_IER_ENABLE_RLS)
    printf("ON\n");
  else
    printf("OFF\n");
}

// FIFOS **********************

int ser_init_fifo_poll(unsigned short base_addr)
{
  // turn on fifo & clear transmit
  unsigned long fcr_config=0;
  fcr_config |= UART_ENABLE_FIFO;
  fcr_config |= UART_FCR_CLEAR_BUFFERS;

  return ser_set_reg(base_addr,UART_FCR,fcr_config);
}

int ser_shut_fifo_poll(unsigned short base_addr)
{
  // desactivate fifo and clear transmit
  unsigned long fcr_config=0;
  fcr_config |= UART_FCR_CLEAR_BUFFERS;

  return ser_set_reg(base_addr,UART_FCR,fcr_config);
}

int ser_send_string_poll_fifo(unsigned short base_addr, unsigned char string[], unsigned int string_size)
{
  int scount = 0, fifo_counter = 0, end_string = 0;

  do
  {
    // get lsr
    unsigned long lsr;
    ser_get_reg(base_addr,UART_LSR,&lsr);

    if (lsr & UART_LSR_THR_EMPTY) // if THR empty, lets send 16 bytes, because we are sure that the FIFO is empty
    {
      while (fifo_counter < 16)
      {
        ser_set_reg(base_addr,UART_THR,(unsigned long) string[scount]); // send char
        //printf("%c",string[scount]);

        fifo_counter++;
        scount++;

        if (scount == string_size) // if we just sent the last char of the string, end function
        {
          end_string = 1;
          break;
        }
      }

      fifo_counter = 0;

    }

  } while (!end_string);

  return 0;
}

int ser_receive_string_poll_fifo(unsigned short base_addr, unsigned char string[], unsigned int string_size)
{
  int scount = 0, fifo_counter = 0, end_string = 0, started_rx = 0, nothing_rx = 0, timeout = 0;

  unsigned long rx_content = '1';

  while(scount < string_size)
  {
    unsigned long lsr;
    ser_get_reg(base_addr,UART_LSR,&lsr);

    // if there is any error
    if (lsr & UART_LSR_OVERRUN_ERROR)
    {
      //printf("Overrun error in serial port data receiving\n");
      return 1;
    }
    else if (lsr & UART_LSR_PARITY_ERROR)
    {
      //printf("Parity Error in serial port data receiving\n");
      return 1;
    }
    else if (lsr & UART_LSR_FRAMING_ERROR)
    {
      //printf("Framing error while receiving string from serial port\n");
      return 1;
    }

    else if (lsr & UART_LSR_RBR_READY) // if there is something to read
    {
      ser_get_reg(base_addr,UART_RBR, &rx_content);
      //printf("%c",rx_content);

      if (!started_rx) // mark that we started receiving the string
        started_rx = 1;

      string[scount] = (unsigned char) rx_content;
      scount++;
    }

    else
    {
      if (started_rx && timeout < 2000) // if we started receiving, wait for more
        timeout++;

      else // nothing to read
        return 1;
    }
  }

  return 0;

}
