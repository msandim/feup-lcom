#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include <minix/sysutil.h>

#include "test7.h"
#include "serial.h"

int ser_test_conf(unsigned short base_addr) {
    ser_get_config();
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
