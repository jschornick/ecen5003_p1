/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  uart.h                                                   --
--                                                                           --
-----------------------------------------------------------------------------*/

#ifndef _UART_H
#define _UART_H

#include "global_types.h"

#define RX_BUF_SIZE 10    /* size of receive buffer in bytes */
#define TX_BUF_SIZE 1000    /* size of transmit buffer in bytes */

// NOTE:  UART0 is also called UARTLP in mbed
#define OERR (UART0->S1 & UARTLP_S1_OR_MASK)   // Overrun Error bit
#define CREN (UART0->C2 & UARTLP_C2_RE_MASK)   // continuous receive enable bit
#define RCREG UART0->D                         // Receive Data Register
#define FERR (UART0->S1 & UARTLP_S1_FE_MASK)   // Framing Error bit
#define RCIF (UART0->S1 & UARTLP_S1_RDRF_MASK) // Receive Interrupt Flag (full)
#define TXIF (UART0->S1 & UARTLP_S1_TDRE_MASK) // Transmit Interrupt Flag (empty)
#define TXREG UART0->D                         // Transmit Data Register
#define TRMT (UART0->S1 & UARTLP_S1_TC_MASK)   // Transmit Shift Register Empty

#define UART_DIRECT 0
#define UART_BUFFERED 1

// points to our current output function (direct or buffered)
extern void (*uart_put)(UCHAR);
void uart_direct_put(UCHAR);
void uart_buffered_put(UCHAR);

// reset buffer pointers and switch to buffered mode
void uart_init(void);

// switch between direct and buffered output
void uart_mode(int);

void uart_poll(void);
UCHAR uart_get(void);
UCHAR uart_input(void);

void uart_msg_put(const char *);
void uart_hex_put(UCHAR);
void uart_word_put(unsigned int);
void uart_dec_put(unsigned int);

UCHAR hex_to_asc(UCHAR c);
UCHAR hex_to_asc(UCHAR c);

#endif
