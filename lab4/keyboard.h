#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n))

#define DELAY_US 20000
#define WAIT_KBC 20

#define KBD_IRQ 1

// ******* ports

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define KBD_IN_BUF 0x60

// ******* bitmasks

// byte with IBF ->1 (bitmask)
#define IBF 0x02
// byte with OBF ->1 (bitmask)
#define OBF 0x01

// PAR BIT
#define PAR_ERR 0x80
// TIMEOUT BIT
#define TO_ERR 0x40

// ******* TYPES OF ERRORS

#define RESEND 0xFE
#define ERROR 0xFC

// ******* KBD COMMANDS

#define ENABLE_KBD_DEFAULT 0xF6
#define TOGGLE_LEDS 0xED

// ******* FUNCTIONS ****************************************************

int keyboard_subscribe_int();

int keyboard_unsubscribe_int();

// executes a cmd to IN_BUF
// returns 0 in success, 1 in non-success
int keyboard_send_cmd(unsigned long cmd, port_t port);

// receives data from OUT_BUF
// returns 0 in success, 1 in non-success
int keyboard_receive_data_kbc(unsigned char *data);

// receives data from OUT_BUF in LEDs (doesnt need to read status to check OBF)
// returns 0 in success, 1 in non-sucess
int keyboard_receive_data_kbd(unsigned char *data);

int keyboard_interrupt_handler(unsigned char *makebreakcode);

// checks if a code is make or break
// returns 2 if it's a 0xE0 byte from the scan code
// returns 1 if it's a break code
// returns 0 if it's a make code
int keyboard_make_or_break(unsigned char code);

// receives a led number, and toggles that led
// returns 0 in success; 1 in non_success
int keyboard_toggle_led(unsigned short led);
#endif
