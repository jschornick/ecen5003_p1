/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  monitor.c                                                --
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

#include "uart.h"
#include "monitor.h"
#include "timer.h"

int stack_flag = 0;
int memory_flag = 0;

#define MSG_BUF_SIZE 10
UCHAR msg_buf[MSG_BUF_SIZE]; // define the storage for UART received messages
UCHAR msg_buf_idx = 0;    // index into the received message buffer

enum dmode display_mode = NORMAL;

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

void display_menu(void)
{
  uart_msg_put("\r\nMain menu\r\n");
  uart_msg_put("N - Normal\r\n");
  uart_msg_put("Q - Quiet\r\n");
  uart_msg_put("D - Debug\r\n" );
  uart_msg_put("V - Version\r\n");
  uart_msg_put("Select:");
}


//******************************************************************************
// UART Input Message
//******************************************************************************
void read_message_from_uart(void)
{
  UCHAR j;
  while( uart_input() )      // while we have unprocessed data in the rx buffer  
  {
    j = uart_get();         // get next character
    msg_buf[msg_buf_idx++] = j;
    process_message();
    break;

    // TODO: continuous mode flag
    // TODO: update_rate variable
    // TODO: memory/stack toggle

      // if multi-char mode...
      if( j == '\r' )         // on a enter (return) key press
      {                       // complete message (all messages end in carriage return)
         uart_msg_put("->");
         process_message();
      }
      else
      {
         if ((j != 0x02) )   // if not ^B
         {                   // if not command, then
            uart_put(j);     // echo the character
         }
         else
         {
           ;
         }
         if( j == '\b' )
         {                          // backspace editor
            if( msg_buf_idx != 0)
            {                       // if not 1st character then destructive
               uart_msg_put(" \b"); // backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )
         {                                // check message length too large
            uart_msg_put("\r\nToo Long!");
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
         }
      }
   }
}

//********************************************************************************************************
// UART Input Message Processing
//********************************************************************************************************
void process_message(void)
{
  UCHAR chr;
  UCHAR err=0;

  chr = msg_buf[0];
  if ((chr >= 'a' ) && (chr <= 'z'))  {
    chr -= ('a' - 'A');  // capitalize
  }

  switch( chr ) {

    case 'D':
      display_mode = DEBUG;
      uart_msg_put("\r\nMode=DEBUG\r\n");
      display_timer = 0;
      break;

    case 'N':
      display_mode = NORMAL;
      uart_msg_put("\r\nMode=NORMAL\r\n");
      //display_timer = 0;
      break;

    case 'Q':
      display_mode = QUIET;
      uart_msg_put("\r\nMode=QUIET\r\n");
      display_timer = 0;
      break;

    case 'V':
      uart_msg_put("\r\n");
      uart_msg_put( CODE_VERSION );
      uart_msg_put("\r\nSelect  ");
      display_timer = 0;
      break;
    default:
      err = 1;
  }

   if( err == 1 )
   {
      display_menu();
   }
   else if( err == 2 )
   {
      uart_msg_put("\n\rNot in DEBUG Mode!");
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

  uart_msg_put("\r\nRegisters:\r\n");
  for(int regnum=0; regnum<REG_COUNT; regnum++) {
    uart_msg_put(" R");
    uart_put('0' + regnum/10);
    uart_put('0' + regnum%10);
    uart_msg_put(": ");
    uart_word_put(regs[regnum]);
    uart_msg_put("\r\n");
  }
}

//******************************************************************************
// ECEN5003 Memory Display
//******************************************************************************

// Display the 32-bit value at the specified memory address.
void display_memory(int addr) {
  int val = *((int *) addr);
  uart_msg_put("\r\nMEM(0x");
  uart_word_put(addr);
  uart_msg_put("):\r\n");
  uart_word_put(val);
  uart_msg_put("\r\n");
}

//******************************************************************************
// ECEN5003 Stack Display
//******************************************************************************

__asm int * get_sp(void) {
  MOV r0, sp
  BX LR
}

void display_stack(void) {
  //cont char max_depth = 16;  // max stack depth to dump
  int *stack_base = (int *) *( (int *) 0x0);  // deference MSP to get top of stack
  //int x;

  uart_msg_put("\r\nStack:\r\n");
  for( volatile int *stack_ptr = get_sp(); stack_ptr < stack_base; stack_ptr++ ) {
    uart_word_put( *stack_ptr );
    uart_msg_put("\r\n");
  }
}

// **********

void display_readings() {
  uart_msg_put(" Flow: ");
  // *** ECEN 5003 add code as indicated ***
  uart_msg_put("  Temp: ");
  uart_msg_put("  Freq: ");
  uart_msg_put("\r\n");
}



/*******************************************************************************
* DEBUG and DIAGNOSTIC Mode UART Operation
*******************************************************************************/
void monitor(void)
{

  switch(display_mode)
  {
    case(QUIET):
      display_flag = 0;
      break;
    case(NORMAL):
      if (display_flag == 1)
        {
          uart_msg_put("\r\nNORMAL ");
          display_readings();
          display_flag = 0;
        }
      break;
    case(DEBUG):
      if (display_flag == 1)
        {
          uart_msg_put("\r\nDEBUG ");
          display_readings();

          /****************  ECEN 5003 add code as indicated  ***************/
          // Create a display of  error counts, sensor states, and ARM Registers R0-R15
          display_registers();
          //  Create a command to read a section of memory and display it
          display_memory(0x0);
          //  Create a command to read 16 words from the current stack
          // and display it in reverse chronological order.
          display_stack();
          display_flag = 0;
      }
      break;
    default:
      uart_msg_put("Mode Error");
  }
}
