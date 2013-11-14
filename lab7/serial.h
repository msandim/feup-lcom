#ifndef _LCOM_SERIAL_H_
#define _LCOM_SERIAL_H_

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

/* BITMASKS FOR LCR */
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
#define UART_LSR_RBR_READY 0x0
#define UART_LSR_OVERRUN_ERROR 0x1
#define UART_LSR_PARITY_ERROR (1<<1)
#define UART_LSR_FRAMING_ERROR (1<<2)
#define UART_LSR_THR_EMPTY (1<<5)

/* BITMASKS FOR IER */
#define UART_IER_RD_INT 0x0
#define UART_IER_TE_INT 0x1
#define UART_IER_RLS_INT 0x2



#endif
