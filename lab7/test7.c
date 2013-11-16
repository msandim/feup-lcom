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

  /* to be done by joao neto */
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
    unsigned long stop, long parity, unsigned long rate,int stringc, char *strings[])
{
  //ser_test_set(base_addr,bits,stop,parity,rate);

  //printf("STRINGC: %u, BASE_ADDR: %x,TX: %x, BITS: %x,STOP: %x,PARITY: %x, RATE: %u",stringc,base_addr,tx,bits,stop,parity,rate);
  //printf("NUMBER OF STRS: %u, STRINGS: %s,%s",stringc,strings[0],strings[1]);

  unsigned int str_count;

  if (tx) // if transmiter

    // send each string!
    for (str_count=0; str_count < stringc; str_count++)
      ser_send_string_poll(base_addr,strings[str_count]);

  else // if receiver
    ser_receive_string_poll(base_addr);
}

int ser_test_int(/* details to be provided */) { 
  /* To be completed */
}

int ser_test_fifo(/* details to be provided */) {
  /* To be completed */
}
