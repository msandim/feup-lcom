#ifndef _LCOM_TEST4_H_
#define _LCOM_TEST4_H_

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n))

#define DELAY_US 20000
#define WAIT_KBC 20

#define KBD_IRQ 1

// ******* ports

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60

// ******* bitmasks

// byte with IBF ->1 (bitmask)
#define IBF 0x02;
// byte with OBF ->1 (bitmask)
#define OBF 0x01;

// PAR BIT
#define PAR_ERR 0x80;
// TIMEOUT BIT
#define TO_ERR 0x40;

// ******* KBD COMMANDS

#define enableKBDdefault 0xF6;

// FUNCTIONS FOR TESTING
int test_scan();
int test_leds(unsigned short n, unsigned short *leds);

// ******* FUNCTIONS

int keyboard_send_kbc_cmd(unsigned long cmd);

int keyboard_receive_data_kbc(unsigned char *data);

int keyboard_subscribe_int();

int keyboard_unsubscribe_int();

void keyboard_int_handler();

#endif
