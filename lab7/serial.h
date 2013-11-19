#ifndef _LCOM_SERIAL_H_
#define _LCOM_SERIAL_H_

#define BIT(n) (0x01<<(n))

#define BITRATE_CONSTANT 115200

#define SER_COM1_IRQ 0x4
#define SER_COM2_IRQ 0x3

/* REGISTER ADDRESSES */
#define SER_PORT_COM1 0x3F8
#define SER_PORT_COM2 0x2F8

#define UART_RBR 0 // Receiver Buffer (read)
#define UART_THR 0 // Transmitter Holding (write)
#define UART_LSR 5 // Line Status (read)
#define UART_LCR 3// Line Control (read/write)
#define UART_IER 1 // Interrupt Enable (read/write)
#define UART_IIR 2 // Interrupt Identification Register (read)
#define UART_FCR 2// FIFO Control (write)

#define UART_DLL 0 // Divisor Latch Least
#define UART_DLM 1 // Divisor Latch Most

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
#define UART_LSR_RBR_READY 0x1
#define UART_LSR_OVERRUN_ERROR 0x2
#define UART_LSR_PARITY_ERROR (1<<2)
#define UART_LSR_FRAMING_ERROR (1<<3)
#define UART_LSR_THR_EMPTY (1<<5)

/* BITMASKS FOR IER */
#define UART_IER_ENABLE_RD 0x1
#define UART_IER_ENABLE_TE 0x2
#define UART_IER_ENABLE_RLS 0x3

/* BITMASKS FOR IIR */
#define UART_INT_PEND 0x1
#define UART_INT_SEQUENCE 0xE

// sequences for interrupts
#define UART_INT_TE 0x2
#define UART_INT_TIMEOUT 0x8
#define UART_INT_RD 0x4
#define UART_INT_LS 0x6

/* ************************* POLLING *****************************/
#define DELAY_POLL 5000 // 5ms

/* FUNCTIONS DECLARATIONS */

int ser_get_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long* result);

int ser_set_reg(unsigned short base_addr, unsigned char relative_addr, unsigned long value);

int ser_get_bit_rate(unsigned short base_addr,unsigned long* bit_rate);

int ser_set_bit_rate(unsigned short base_addr,unsigned long bit_rate);

void ser_show_lcr(unsigned long lcr);

void ser_show_ier(unsigned long ier);

/* FOR POLLING */

int ser_send_string_poll(unsigned short base_addr, char string[]);

void ser_send_char_poll(unsigned short base_addr,unsigned char char_send);

int ser_receive_string_poll(unsigned short base_addr);

int ser_receive_char_poll(unsigned short base_addr, unsigned char* char_receive);

/* FOR INTERRUPTS */
int ser_send_string_int(unsigned short base_addr, char string[]);

int ser_receive_string_int(unsigned short base_addr);

int ser_ih(unsigned short base_addr, unsigned char* char_send_receive);

int ser_subscribe_int(unsigned short base_addr);

int ser_unsubscribe_int();

#endif
