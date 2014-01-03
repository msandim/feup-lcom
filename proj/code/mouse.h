#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

/** @defgroup mouse Mouse
 * @{
 *
 * Low level module that has several functions that work directly with the mouse
 */

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n))

#define DELAY_MOUSE 10000
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

// ******* MOUSE FUNCTIONS ****************************************************

/**
 * @brief Subscribes and enables mouse interrupts
 *
 * Returns a bit mask with with bit 1 selected on the interrupt type.
 *
 * @return the bit mask, or -1 if it's not successful
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribes mouse interrupts
 *
 * @return 0 on success, non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Sends a command to the mouse
 *
 * This function sends the WRITE_BYTE_MOUSE command to KBC_CMD_REG before sending the command to MOUSE_CMD_PORT
 *
 * @param cmd Command code to send
 *
 * @return 0 on success, non-zero otherwise
 */
int mouse_send_cmd(unsigned long cmd);


/**
 * @brief Receives a byte from the OUT_BUF
 * @param data pointer to the unsigned char to fill with the byte coming from the OUT_BUF
 * @return 0 on success, non-zero otherwise
 */
int mouse_receive_data_outbuf(unsigned char *data);

/**
 * @brief Mouse Interrupt Handler
 *
 * This handler syncs the bytes coming from the OUT_BUF, and saves them in a packet (array in mouse.c)
 */
void mouse_interrupt_handler();

/**
 * @brief Checks if the packet is valid
 *
 * This function checks if the "packet" array in mouse.c is a valid packet (bit 3 of packet[0] is set to 1)
 * @return 0 on false, non-zero on true
 */
int mouse_valid_packet();

/**
 * @brief Checks if the packet is complete (and checks if it's valid)
 *
 * This function checks if the "packet" array in mouse.c is a valid packet (bit 3 of packet[0] is set to 1)
 * and if the count variable is set to 0 (packet ended signal)
 * @return 0 on false, non-zero on true
 */
int mouse_ended_packet();

/**
 * @brief Returns the packet received
 *
 * @return pointer to the first element of the array "packet"
 */
unsigned char * mouse_get_packet();

/**
 * @brief Receives the config from the mouse registers
 * @param byte1 pointer to the first byte of config to be changed
 * @param byte2 pointer to the second byte of config to be changed
 * @param byte3 pointer to the third byte of config to be changed
 * @return 0 on success, non-zero otherwise
 */
int mouse_get_config(unsigned char *byte1, unsigned char *byte2, unsigned char *byte3);

/**
 * @brief Shows the configuration of the mouse in an human friendly way
 * @param byte1 first byte of config
 * @param byte2 second byte of config
 * @param byte3 third byte of config
 */
void mouse_show_config(unsigned char byte1, unsigned char byte2, unsigned char byte3);

/**
 * @brief Changes the state to exit in lab5 (function designed for lab5)
 * @return 1 if it reaches the final state, 0 otherwise
 */
int mouse_exit_handler();

/**
 * @brief Prints the current packet in a human friendly way
 */
void mouse_print_packet();

/** @} */

#endif
