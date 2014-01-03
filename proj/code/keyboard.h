#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H

/** @defgroup keyboard Keyboard
 * @{
 *
 * Low level module that has several functions that work directly with the keyboard
 */

// ******* constants to use in keyboard

#define BIT(n) (0x01<<(n)) /**< @brief Macro to generate bitmaks */

#define DELAY_KBD 20000 /**< @brief Delay to use in the KBD functions */
#define WAIT_KBC 20 /**< @brief Delay to use in the KBC functions */

#define KBD_IRQ 1 /**< @brief KBD IRQ */

// ******* ports

#define STAT_REG 0x64 /**< @brief STAT_REG port address */
#define KBC_CMD_REG 0x64 /**< @brief KBC_CMD_REG port address */
#define OUT_BUF 0x60 /**< @brief OUT_BUF port address */
#define KBD_IN_BUF 0x60 /**< @brief KBD_IN_BUF port address */

// ******* bitmasks

// byte with IBF ->1 (bitmask)
#define IBF 0x02 /**< @brief Bitmask with IBF to 1*/
// byte with OBF ->1 (bitmask)
#define OBF 0x01 /**< @brief Bitmask with OBF to 1*/

// PAR BIT
#define PAR_ERR 0x80 /**< @brief Bitmask to par error bit*/
// TIMEOUT BIT
#define TO_ERR 0x40 /**< @brief Bitmask to timeout error bit*/

// ******* TYPES OF ERRORS

#define RESEND 0xFE /**< @brief Response of type "RESEND" */
#define ERROR 0xFC /**< @brief Response of type "ERROR" */

// ******* KBD COMMANDS

#define ENABLE_KBD_DEFAULT 0xF6 /**< @brief ENABLE_KBD_DEFAULT command */
#define TOGGLE_LEDS 0xED /**< @brief TOGGLE_LEDS command */

// ******* FUNCTIONS ****************************************************

/**
 * @brief Subscribes and enables keyboard interrupts
 *
 * Returns a bit mask with with bit 1 selected on the interrupt type.
 *
 * @return the bit mask, or -1 if it's not successful
 */
int keyboard_subscribe_int();

/**
 * @brief Unsubscribes keyboard interrupts
 *
 * @return 0 on success, non-zero otherwise
 */
int keyboard_unsubscribe_int();

/**
 * @brief Sends a command to IN_BUF, to a port defined as parameter
 *
 * @param cmd command code to send
 * @param port port where to send the command
 *
 * @return 0 on success, non-zero otherwise
 */
int keyboard_send_cmd(unsigned long cmd, port_t port);

/**
 * @brief Receives a command (byte) from the OUT_BUF kbc
 *
 * @param data pointer to the byte to be filled with the command received
 *
 * @return 0 on success, non-zero otherwise
 */
int keyboard_receive_data_kbc(unsigned char *data);

/**
 * @brief Receives a command (byte) from the OUT_BUF kbd
 *
 * @param data pointer to the byte to be filled with the info received
 *
 * @return 0 on success, non-zero otherwise
 */
int keyboard_receive_data_kbd(unsigned char *data);

/**
 * @brief Keyboard Interrupt Handler
 *
 * Handles interrupts from the keyboard, returning the char received through the makebreakcode parameter
 *
 * @param makebreakcode pointer to the byte to be filled with the info received
 *
 * @return type of code received (returns keyboard_make_or_break() invocation with the code received)
 */
int keyboard_interrupt_handler(unsigned char *makebreakcode);

/**
 * @brief Analyzes if a code is a makecode, breakcode or a 2 byte code indicator (0xE0)
 *
 * @param code byte to be analyzed
 *
 * @return returns 2 if it's a 0xE0 byte (indicating 2 byte code) from the scan code;
 *         returns 1 if it's a break code;
 *         returns 0 if it's a make code
 */
int keyboard_make_or_break(unsigned char code);


/**
 * @brief Toggles the kbd's leds
 *
 * @param led number of the led to toggle
 *
 * @return 0 on success, non-zero otherwise
 */
int keyboard_toggle_led(unsigned short led);

/** @} */

#endif
