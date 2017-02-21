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

// initialize serial buffer pointers
void uart_init(void);

void uart_poll(void);
void uart_put(UCHAR);
UCHAR uart_get(void);
UCHAR uart_input(void);
void uart_direct_msg_put(const char *);
void uart_msg_put(const char *);
void uart_direct_hex_put(UCHAR);
void uart_direct_put(UCHAR);
void uart_hex_put(UCHAR);
void uart_hex_word_put(unsigned int);

#endif
