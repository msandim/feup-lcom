#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test7.h"
#include "serial.h"

int ser_test_conf(unsigned short base_addr) {

  unsigned long lcr_config, ier_config;
  if(ser_get_lcr(base_addr,&lcr_config) || ser_get_ier(base_addr,&ier_config))
  {
    printf("Error reading config\n\n");
    return 1;
  }

  ser_show_lcr(lcr_config);
  ser_show_ier(ier_config);

  return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
    long parity, unsigned long rate) {
  /* To be completed */
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
    unsigned long stop, long parity, unsigned long rate,
    int stringc, char *strings[]) {
  /* To be completed */
}

int ser_test_int(/* details to be provided */) { 
  /* To be completed */
}

int ser_test_fifo(/* details to be provided */) {
  /* To be completed */
}
