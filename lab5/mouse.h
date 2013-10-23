#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n))

#define DELAY_US 20000
#define WAIT_KBC 20

#define MOUSE_IRQ 12

// ******* ports

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define KBC_IN_BUF 0x60

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

int mouse_subscribe_int();

int mouse_unsubscribe_int();

// executes a cmd to IN_BUF
// returns 0 in success, 1 in non-success
int moused_send_cmd(unsigned long cmd, port_t port);

// receives data from OUT_BUF
// returns 0 in success, 1 in non-success
int mouse_receive_data_kbc(unsigned char *data);

// receives data from OUT_BUF in LEDs (doesnt need to read status to check OBF)
// returns 0 in success, 1 in non-sucess
int keyboard_receive_data_kbd(unsigned char *data);



#endif
