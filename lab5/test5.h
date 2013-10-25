#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n))

#define DELAY_US 10000
#define WAIT_KBC 20

#define MOUSE_IRQ 12

// ******* ports

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define MOUSE_CMD_PORT 0x60

// ******* bitmasks

// byte with IBF ->1 (bitmask)
#define IBF 0x02
// byte with OBF ->1 (bitmask)
#define OBF 0x01

// PAR BIT
#define PAR_ERR 0x80
// TIMEOUT BIT
#define TO_ERR 0x40

// - for MOUSE:
#define bit3_mask 0x08
#define ret_mask 0x03 // has left + right
#define MB_mask 0x04
#define RB_mask 0x02
#define LB_mask 0x01
#define XOV_mask 0x40
#define YOV_mask 0x80
#define XSIGN_mask 0x10
#define YSIGN_mask 0x20

//    - for config bytes
#define mode_mask 0x40
#define enable_mask 0x20
#define scaling_mask 0x10
#define leftbtn_mask 0x04
#define middlebtn_mask 0x02
#define rightbtn_mask 0x01

// ******* TYPES OF ERRORS

#define NACK_RESEND 0xFE
#define ERROR 0xFC
#define ACK 0xFA

// ******* KBC COMMANDS

#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_PACKETS 0xF4
#define DISABLE_STREAM_MODE 0xF5
#define STATUS_REQUEST 0xE9

// FUNCTIONS FOR TESTING
int test_packet();
int test_asynch(unsigned short duration);
int test_config(void);

// ******* MOUSE FUNCTIONS ****************************************************

// Subscribes and enables mouse interrupts
// Returns a bit order in interrupt mask if sucessful, -1 if not
int mouse_subscribe_int();

// Unsubscribes mouse interrupts
// Returns 0 if sucessful, non-zero if not
int mouse_unsubscribe_int();

// executes a cmd to IN_BUF
// returns 0 in success, 1 in non-success
int moused_send_cmd(unsigned long cmd, port_t port);

// receives data from OUT_BUF
// returns 0 in success, 1 in non-success
int mouse_receive_data_outbuf(unsigned char *data);

void mouse_interrupt_handler();

int mouse_get_config(unsigned char *byte1, unsigned char *byte2, unsigned char *byte3);

int mouse_exit_handler();

void mouse_print_packet();


#endif
