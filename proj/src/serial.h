#ifndef _LCOM_SERIAL_H_
#define _LCOM_SERIAL_H_

/** @defgroup serial Serial Port (UART)
 * @{
 *
 * Low level module that has several functions that work directly with the UART
 */

#define BIT(n) (0x01<<(n))

#define BITRATE_CONSTANT 115200 /**< @brief Bitrate constant */

#define SER_COM1_IRQ 0x4 /**< @brief IRQ to COM1 */
#define SER_COM2_IRQ 0x3 /**< @brief IRQ TO COM2 */

/* REGISTER ADDRESSES */
#define SER_PORT_COM1 0x3F8 /**< @brief COM1 Address */
#define SER_PORT_COM2 0x2F8 /**< @brief COM2 Address */

#define UART_RBR 0 /**< @brief Receiver Buffer (read) */
#define UART_THR 0 /**< @brief Transmitter Holding (write) */
#define UART_LSR 5 /**< @brief Line Status (read) */
#define UART_LCR 3 /**< @brief Line Control (read/write) */
#define UART_IER 1 /**< @brief Interrupt Enable (read/write) */
#define UART_IIR 2 /**< @brief Interrupt Identification Register (read) */
#define UART_FCR 2 /**< @brief FIFO Control (write) */

#define UART_DLL 0 /**< @brief Divisor Latch Least */
#define UART_DLM 1 /**< @brief Divisor Latch Most */

/* BITMASKS FOR UART LCR */
#define UART_LCR_WORD_LENGTH 0x3
#define UART_LCR_STOP_BIT 0x4
#define UART_LCR_PARITY 0x38
#define UART_LCR_PARITY_BIT_3 0x8
#define UART_LCR_SET_BREAK 0x40
#define UART_LCR_DLAB 0x80

/* SEQUENCES FOR UART LCR */
// Word size in Line Control
#define UART_LCR_WORD_LENGTH_5 0x0
#define UART_LCR_WORD_LENGTH_6 0x1
#define UART_LCR_WORD_LENGTH_7 0x2
#define UART_LCR_WORD_LENGTH_8 0x3

// number of stop bits
#define UART_LCR_STOP_BIT_1 0x0
#define UART_LCR_STOP_BIT_2 0x4

// parity
#define UART_LCR_NO_PARITY 0x0
#define UART_LCR_ODD_PARITY 0x08
#define UART_LCR_EVEN_PARITY 0x18
#define UART_LCR_1_PARITY 0x28
#define UART_LCR_0_PARITY 0x38

// DLAB
#define UART_LCR_DLAB_DL 0x80
#define UART_LCR_DLAB_DATA 0x00

/* BITMASKS FOR LSR */
#define UART_LSR_RBR_READY 0x1 /**< @brief Bit mask to bit 0 in LSR (Receiver Ready) */
#define UART_LSR_OVERRUN_ERROR 0x2 /**< @brief Bit mask to bit 0 in LSR (Overrun Error) */
#define UART_LSR_PARITY_ERROR (1<<2) /**< @brief Bit mask to bit 0 in LSR (Parity Error) */
#define UART_LSR_FRAMING_ERROR (1<<3) /**< @brief Bit mask to bit 0 in LSR (Framing Error) */
#define UART_LSR_THR_EMPTY (1<<5) /**< @brief Bit mask to bit 0 in LSR (Transmiter Register Empty) */

/* BITMASKS FOR IER */
#define UART_IER_ENABLE_RD 0x1
#define UART_IER_ENABLE_TE 0x2
#define UART_IER_ENABLE_RLS 0x4

/* BITMASKS FOR IIR */
#define UART_INT_PEND 0x1
#define UART_INT_SEQUENCE 0xE

// sequences for interrupts
#define UART_INT_TE 0x2
#define UART_INT_TIMEOUT 0xC
#define UART_INT_RD 0x4
#define UART_INT_LS 0x6

/* BITMASKS FOR FCR */
#define UART_ENABLE_FIFO 0x1 /**< @brief Bit mask to enable FIFO in FCR */
#define UART_FCR_CLEAR_BUFFERS 0x6 /**< @brief Bit mask to clear XMIT and TMIT buffers in FCR */
#define UART_CLEAR_RFIFO 0x2 /**< @brief Bit mask to clear XMIT buffer in FCR */
#define UART_CLEAR_TFIFO 0x4 /**< @brief Bit mask to clear TMIT buffer in FCR */

#define UART_TRIGGER_LVL_1 0x00
#define UART_TRIGGER_LVL_4 0x40
#define UART_TRIGGER_LVL_8 0x80
#define UART_TRIGGER_LVL_14 0xC0


/* ************************* POLLING *****************************/
#define DELAY_POLL 5000 // 5ms

/* FUNCTIONS DECLARATIONS */

// gets the value of a reg with relative addr/ returns 1 in success, 0 in non-success
int ser_get_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long* result);

// sets the value of a reg with relative addr/ returns 1 in success, 0 in non-success
int ser_set_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long value);

// gets the value of bite rate/ returns 1 in success, 0 in non-success
int ser_get_bit_rate(unsigned short base_addr,unsigned long* bit_rate);

// sets the value of bite rate/ returns 1 in success, 0 in non-success
int ser_set_bit_rate(unsigned short base_addr,unsigned long bit_rate);

// sets config
int ser_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop, long parity, unsigned long rate);

// shows lcr in a human friendly way
void ser_show_lcr(unsigned long lcr);

// shows ier in a human friendly way
void ser_show_ier(unsigned long ier);

/* FOR POLLING */

// sends an array of bytes in polling mode/ returns 1 in success, 0 in non-success
int ser_send_string_poll(unsigned short base_addr, unsigned char string[]);

// sends a byte in polling mode
void ser_send_char_poll(unsigned short base_addr,unsigned char char_send);

// receives an array of bytes in polling mode/ returns 1 in success, 0 in non-success
int ser_receive_string_poll(unsigned short base_addr);

// receives a byte in polling mode/ returns 1 in success, 0 in non-success
int ser_receive_char_poll(unsigned short base_addr, unsigned char* char_receive);

/* FOR INTERRUPTS */

// sends a string by interrupt mode/ returns 1 in success, 0 in non-success
int ser_send_string_int(unsigned short base_addr, unsigned char string[]);

// receives a string by interrupt mode/ returns 1 in success, 0 in non-success
int ser_receive_string_int(unsigned short base_addr);

/* FOR FIFOS */

// sends a string by interrupt mode with fifos/returns 1 in success, 0 in non-success
int ser_send_string_int_fifo(unsigned short base_addr,unsigned char string[]);

// receive a string by interrupt mode with fifos/ returns 1 in success, 0 in non-success
int ser_receive_string_int_fifo(unsigned short base_addr,unsigned long trigger);

// initiates fifo activity prepared for polling mode
int ser_init_fifo_poll(unsigned short base_addr);

// shuts down fifo activity prepared for polling mode
int ser_shut_fifo_poll(unsigned short base_addr);

// send a string by polling mode with fifos/ returns 1 in success, 0 in non-success
int ser_send_string_poll_fifo(unsigned short base_addr, unsigned char string[], unsigned int string_size);

int ser_receive_string_poll_fifo(unsigned short base_addr, unsigned char string[], unsigned int string_size);

/* OTHERS */

// Interrupt Handler: handles serial port interrupts
// Returns: 0 if receives or returns a byte or an array of bytes (if using fifos)
//          1 if receives an error
//          2 if a timeout interrupt occurs
//          3 if a interrupt outside the serial port happens
int ser_ih(unsigned short base_addr, unsigned char* char_send_receive, int fifo, int size_fifo);

int ser_subscribe_int(unsigned short base_addr);

int ser_unsubscribe_int();

/** @} */

#endif
