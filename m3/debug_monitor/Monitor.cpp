/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      Monitor.c                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--
--
--  Designed by:  Tim Scherr
--  Revised by :  Jeff Schornick
--
-- Version: 2.0
-- Date of current revision:  2016-09-29
-- Target Microcontroller: Freescale MKL25ZVMT4
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--
--
   Functional Description: See below
--
-- Copyright (c) 2015 Tim Scherr All rights reserved.
--
*/

#include <stdio.h>
#include "shared.h"

/*******************************************************************************
* Set Display Mode Function
* Function determines the correct display mode.  The 3 display modes operate as
*   follows:
*
*  NORMAL MODE       Outputs only mode and state information changes
*                     and calculated outputs
*
*  QUIET MODE        No Outputs
*
*  DEBUG MODE        Outputs mode and state information, error counts,
*                    register displays, sensor states, and calculated output
*
*
* There is deliberate delay in switching between modes to allow the RS-232 cable
* to be plugged into the header without causing problems.
*******************************************************************************/

void set_display_mode(void)
{
  UART_direct_msg_put("\r\nSelect Mode");
  UART_direct_msg_put("\r\n Hit NOR - Normal");
  UART_direct_msg_put("\r\n Hit QUI - Quiet");
  UART_direct_msg_put("\r\n Hit DEB - Debug" );
  UART_direct_msg_put("\r\n Hit V - Version#\r\n");
  UART_direct_msg_put("\r\nSelect:  ");
}


//******************************************************************************
// UART Input Message
//******************************************************************************
void chk_UART_msg(void)
{
   UCHAR j;
   while( UART_input() )      // becomes true only when a byte has been received
   {                          // skip if no characters pending
      j = UART_get();         // get next character

      if( j == '\r' )         // on a enter (return) key press
      {                       // complete message (all messages end in carriage return)
         UART_msg_put("->");
         UART_msg_process();
      }
      else
      {
         if ((j != 0x02) )   // if not ^B
         {                   // if not command, then
            UART_put(j);     // echo the character
         }
         else
         {
           ;
         }
         if( j == '\b' )
         {                          // backspace editor
            if( msg_buf_idx != 0)
            {                       // if not 1st character then destructive
               UART_msg_put(" \b"); // backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )
         {                                // check message length too large
            UART_msg_put("\r\nToo Long!");
            msg_buf_idx = 0;
         }
         else if ((display_mode == QUIET) && (msg_buf[0] != 0x02) &&
                  (msg_buf[0] != 'D') && (msg_buf[0] != 'N') &&
                  (msg_buf[0] != 'V') &&
                  (msg_buf_idx != 0))
         {                          // if first character is bad in Quiet mode
            msg_buf_idx = 0;        // then start over
         }
         else {                     // not complete message, store character

            msg_buf[msg_buf_idx] = j;
            msg_buf_idx++;
            if (msg_buf_idx > 2)
            {
               UART_msg_process();
            }
         }
      }
   }
}

//********************************************************************************************************
// UART Input Message Processing
//********************************************************************************************************
void UART_msg_process(void)
{
   UCHAR chr,err=0;

   if( (chr = msg_buf[0]) <= 0x60 )
   {      // Upper Case
      switch( chr )
      {
         case 'D':
            if((msg_buf[1] == 'E') && (msg_buf[2] == 'B') && (msg_buf_idx == 3))
            {
               display_mode = DEBUG;
               UART_msg_put("\r\nMode=DEBUG\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'N':
            if((msg_buf[1] == 'O') && (msg_buf[2] == 'R') && (msg_buf_idx == 3))
            {
               display_mode = NORMAL;
               UART_msg_put("\r\nMode=NORMAL\n");
               //display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'Q':
            if((msg_buf[1] == 'U') && (msg_buf[2] == 'I') && (msg_buf_idx == 3))
            {
               display_mode = QUIET;
               UART_msg_put("\r\nMode=QUIET\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'V':
            display_mode = VERSION;
            UART_msg_put("\r\n");
            UART_msg_put( CODE_VERSION );
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;

         default:
            err = 1;
      }
   }

   else
   {                 // Lower Case
      switch( chr )
      {
        default:
         err = 1;
      }
   }

   if( err == 1 )
   {
      UART_msg_put("\n\rError!");
   }
   else if( err == 2 )
   {
      UART_msg_put("\n\rNot in DEBUG Mode!");
   }
   else
   {
    msg_buf_idx = 0;  // put index to start of buffer for next message
   }
   msg_buf_idx = 0;  // put index to start of buffer for next message
}


//*****************************************************************************
// IS_HEX Function
// Function takes a single ASCII character and returns
//   1 if hex digit, 0 otherwise.
//*****************************************************************************
UCHAR is_hex(UCHAR c)
{
   if( (((c |= 0x20) >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))  )
      return 1;
   return 0;
}


//******************************************************************************
// ECEN5003 Register Display
//******************************************************************************

// Get register r0. No parameters to disturb its value.
__asm int r0(void) {
  BX lr
}

// Dump registers r1-r15 to memory location specified by r0
__asm void get_regs(int *regs) {
  STM r0!, {r1-r7}  ; store the lo registers
  MOV r1, r8
  STR r1, [r0]      ; store r8 (r0 was advanced by STM)
  MOV r1, r9
  STR r1, [r0, #4]
  MOV r1, r10
  STR r1, [r0, #8]
  MOV r1, r11
  STR r1, [r0, #12]
  MOV r1, r12
  STR r1, [r0, #16]
  MOV r1, r13
  STR r1, [r0, #20]
  MOV r1, r14
  STR r1, [r0, #24]
  MOV r1, r15
  STR r1, [r0, #28]
  BX lr
}

// Display the CPU registers.
// Note that the reported values of r13-r15 (sp, lr, pc) will be tainted by
// the call to this function.
void display_registers(void) {
  const char REG_COUNT=16;
  int regs[REG_COUNT];

  regs[0] = r0();
  get_regs(regs+1); // r1-15

  UART_msg_put("\r\nRegisters:\r\n");
  for(int regnum=0; regnum<REG_COUNT; regnum++) {
    UART_msg_put(" R");
    UART_put('0' + regnum/10);
    UART_put('0' + regnum%10);
    UART_msg_put(": ");
    UART_hex_word_put(regs[regnum]);
    UART_msg_put("\r\n");
  }
}

//******************************************************************************
// ECEN5003 Memory Display
//******************************************************************************

// Display the 32-bit value at the specified memory address.
void display_memory(int addr) {
  int val = *((int *) addr);
  UART_msg_put("\r\nMEM(0x");
  UART_hex_word_put(addr);
  UART_msg_put("):\r\n");
  UART_hex_word_put(val);
  UART_msg_put("\r\n");
}

//******************************************************************************
// ECEN5003 Memory Display
//******************************************************************************

void display_stack(void) {
}


/*******************************************************************************
* DEBUG and DIAGNOSTIC Mode UART Operation
*******************************************************************************/
void monitor(void)
{

  switch(display_mode)
  {
    case(QUIET):
      UART_msg_put("\r\n ");
      display_flag = 0;
      break;
    case(VERSION):
      display_flag = 0;
      break;
    case(NORMAL):
      if (display_flag == 1)
        {
          UART_msg_put("\r\nNORMAL ");
          UART_msg_put(" Flow: ");
          // *** ECEN 5003 add code as indicated ***
          // add flow data output here, use UART_hex_put or similar for numbers
          UART_msg_put(" Temp: ");
          //  add flow data output here, use UART_hex_put or similar for numbers
          UART_msg_put(" Freq: ");
          // add flow data output here, use UART_hex_put or similar for numbers
          display_flag = 0;
        }
      break;
    case(DEBUG):
      if (display_flag == 1)
        {
          UART_msg_put("\r\nDEBUG ");
          UART_msg_put(" Flow: ");
          // ECEN 5003 add code as indicated
          // add flow data output here, use UART_hex_put or similar for numbers
          UART_msg_put(" Temp: ");
          // add flow data output here, use UART_hex_put or similar for numbers
          UART_msg_put(" Freq: ");
          // add flow data output here, use UART_hex_put or similar for numbers

          /****************  ECEN 5003 add code as indicated  ***************/
          // Create a display of  error counts, sensor states, and ARM Registers R0-R15
          display_registers();
          //  Create a command to read a section of memory and display it
          display_memory(0x0);
          //  Create a command to read 16 words from the current stack
          // and display it in reverse chronological order.
          display_stack();

          // clear flag to ISR
          display_flag = 0;
      }
      break;
    default:
      UART_msg_put("Mode Error");
  }
}
