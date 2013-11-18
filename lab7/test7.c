#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test7.h"
#include "serial.h"

int ser_test_conf(unsigned short base_addr) {

  unsigned long lcr_config, ier_config, bit_rate;
  if(ser_get_reg(base_addr,UART_LCR,&lcr_config) || ser_get_reg(base_addr,UART_IER,&ier_config))
  {
    printf("Error reading config\n\n");
    return 1;
  }

  if (ser_get_bit_rate(base_addr,&bit_rate))
  {
    printf("Error reading bit rate");
    return 1;
  }

  ser_show_lcr(lcr_config);
  ser_show_ier(ier_config);

  printf("Bite rate: %d bps\n", bit_rate);

  return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
    long parity, unsigned long rate) {

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

  ser_set_bit_rate(base_addr, rate);

  //printf("BASE_ADDR: %x, BITS: %x,STOP: %x,PARITY: %d, RATE: %u",base_addr,bits,stop,parity,rate);

  return 0;
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
    unsigned long stop, long parity, unsigned long rate,int stringc, char *strings[])
{
  // save current lcr and rate
  unsigned long lcr_backup, rate_backup;
  ser_get_reg(base_addr,UART_LCR,&lcr_backup);
  ser_get_bit_rate(base_addr,&rate_backup);

  ser_test_set(base_addr,bits,stop,parity,rate);

  // test (maybe deactivate ALL interrupts before it ?!) ********************
  unsigned long ier = 0;
  ser_set_reg(base_addr,UART_IER,ier);

  //printf("STRINGC: %u, BASE_ADDR: %x,TX: %x, BITS: %x,STOP: %x,PARITY: %x, RATE: %u\n",stringc,base_addr,tx,bits,stop,parity,rate);
  //printf("NUMBER OF STRS: %u, STRINGS: %s,%s,%s,%s\n",stringc,strings[0],strings[1],strings[2],strings[3]);

  unsigned int str_count;

  if (tx) /* if transmiter */ {

    // send each string!
    for (str_count=0; str_count < stringc; str_count++)
    {
      ser_send_string_poll(base_addr,strings[str_count]);

      if (str_count != stringc-1) // if not the last string, send space to separate
        ser_send_char_poll(base_addr,' ');
    }

    ser_send_char_poll(base_addr,'.'); // send a space to separate strings

  }
  else // if receiver
    ser_receive_string_poll(base_addr);

  // reset lcr and rate previously saved
  ser_set_reg(base_addr,UART_LCR,lcr_backup);
  ser_set_bit_rate(base_addr,rate_backup);

}

int ser_test_int(/* details to be provided */) { 
  /* To be completed */
}

int ser_test_fifo(/* details to be provided */) {
  /* To be completed */
}
