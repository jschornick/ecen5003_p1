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

#include "system_MKL25Z4.h"
#include "uart.h"
#include "monitor.h"
#include "timer.h"
#include "adc.h"
#include "flow_calc.h"

int input_mode = 0; // set to 1 for multi-letter input

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
  uart_msg_put("\r\nMain\r\n");
  uart_msg_put("N - Normal\r\n");
  uart_msg_put("Q - Quiet\r\n");
  uart_msg_put("D - Debug\r\n" );
  uart_msg_put("A - ADCs\r\n");
  uart_msg_put("I - SysInfo\r\n");
  uart_msg_put("V - Version\r\n");
  uart_msg_put("-> ");
}

void debug_menu(void)
{
  uart_msg_put("\r\nDebug\r\n");
  uart_msg_put("R - Registers\r\n");
  uart_msg_put("M - Memory\r\n");
  uart_msg_put("S - Stack\r\n" );
  uart_msg_put("F - Flow Data\r\n");
  uart_msg_put("I - SysInfo\r\n");
  uart_msg_put("V - Version\r\n");
  uart_msg_put("N - Normal\r\n");
  uart_msg_put("-> ");
}


//******************************************************************************
// UART Input Message
//******************************************************************************
void read_message_from_uart(void)
{
  UCHAR j;
  while( uart_input() )  // while we have unprocessed data in the rx buffer
  {
    j = uart_get();      // get next character
    if (!input_mode) {
      msg_buf[msg_buf_idx++] = j;
      process_message();
    }

    else { // multi-char mode...
      if( j == '\r' ) { // "enter" key
        // complete message (all messages end in carriage return)
        msg_buf[msg_buf_idx++] = 0;  // null terminate
        process_message();
      } else {
        // continue adding to message
        if( (j == '\b') || (j == 127) ) {
          // backspace or del
          if( msg_buf_idx != 1) {
            // if not 1st char of the input param
            // (char 0 is the command letter)
            uart_msg_put("\b \b"); // backspace
            msg_buf_idx--;
          }
        }
        else if( msg_buf_idx < MSG_BUF_SIZE ) {
          // we're below max length, so store character
          msg_buf[msg_buf_idx] = j;
          msg_buf_idx++;
          if ((j != 0x02) ) {
            // echo if not command char
            uart_put(j);
          }
        }
      }
    } // multi char

  } // while input
}

//********************************************************************************************************
// UART Input Message Processing
//********************************************************************************************************
void process_message(void)
{
  UCHAR chr;
  chr = msg_buf[0];
  if ((chr >= 'a' ) && (chr <= 'z'))  {
    chr -= ('a' - 'A');  // capitalize
  }

  switch( display_mode ) {
    case NORMAL:
    case QUIET:
    case ADC:
      switch( chr ) {
        case 'A':
          display_mode = ADC;
          uart_msg_put("\r\nMode -> ADC\r\n");
          display_timer = 0;
        break;
        case 'D':
          uart_msg_put("\r\nMode -> DEBUG\r\n");
          display_mode = DEBUG;
          debug_menu();
          break;
        case 'N':
          display_mode = NORMAL;
          uart_msg_put("\r\nMode -> NORMAL\r\n");
          break;
        case 'Q':
          display_mode = QUIET;
          uart_msg_put("\r\nMode -> QUIET\r\n");
          break;
        case 'I':
          display_sysinfo();
          break;
        case 'V':
          display_version();
          break;
        case '?':
          display_menu();
          break;
        default:
          uart_msg_put("\r\nMain (? menu) -> ");
      }
      break;
    case DEBUG:
      switch( chr ) {
        case 'R':
          display_registers();
          break;
        case 'M':
          if (msg_buf_idx == 1) {
            uart_msg_put("\r\nAddr? -> ");
            input_mode = 1;  // get memory param
          } else {
            input_mode = 0;
            display_memory();
          }
          break;
        case 'S':
          display_stack();
          break;
        case 'F':
          display_readings();
          uart_msg_put("\r\n");
          break;
        case 'I':
          display_sysinfo();
          break;
        case 'N':
          display_mode = NORMAL;
          uart_msg_put("\r\nMode -> Normal\r\n");
          break;
        case 'V':
          display_version();
          break;
        case '?':
          debug_menu();
          break;
        default:
          uart_msg_put("\r\nDebug (? menu) -> ");
      }
  }
  if(!input_mode){
    msg_buf_idx = 0;  // put index to start of buffer for next message
    display_timer = 0;
  }
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
void display_memory() {
  unsigned int addr = 0x0;
  unsigned int data;
  unsigned char chr;
  int digit;

  for(int i = 1; i<9; i++) {
    chr = msg_buf[i];
    if( chr == 0 ) { break; } // null
    digit = -1;
    if( (chr >= '0') & (chr <= '9') ) {
      digit = chr - '0';
    }
    if( (chr >= 'A') & (chr <= 'F') ) {
      digit = 10 + chr - 'A';
    }
    if( (chr >= 'a') & (chr <= 'f') ) {
      digit = 10 + chr - 'a';
    }
    if( digit < 0 ) {
      uart_msg_put("\r\nBad memory location!\r\n");
      return;
    }
    addr <<=4;
    addr += digit;
  }
  uart_msg_put("\r\nMemory:\r\n");
  for(int i=addr; i < addr+32; i += 4) {
    uart_msg_put("Ox");
    uart_word_put(i);
    uart_msg_put(" : ");
    data = *((int *) i);
    uart_word_put(data);
    uart_msg_put("\r\n");
  }
}

//******************************************************************************
// ECEN5003 Stack Display
//******************************************************************************

__asm int * get_sp(void) {
  MOV r0, sp
  BX LR
}

void display_stack(void) {
  int *stack_base = (int *) *( (int *) 0x0);  // deference MSP to get top of stack

  uart_msg_put("\r\nStack (recent first):\r\n");
  for( volatile int *stack_ptr = get_sp(); stack_ptr < stack_base; stack_ptr++ ) {
    uart_word_put( *stack_ptr );
    uart_msg_put("\r\n");
  }
}

void display_readings() {
  // *** ECEN 5003 add code as indicated ***
  uart_msg_put("\r\n");
  uart_msg_put(" Flow: ");
  uart_dec_put(flow);
  uart_msg_put("  Temp: ");
  uart_dec_put(convert_temp(adc_vals[2]));
  uart_msg_put("  Freq: ");
  uart_dec_put(freq);
}


void display_sysinfo() {
  uart_msg_put("\r\n");
  uart_msg_put("System Information:\r\n");
  uart_msg_put(" Core clock: ");
  uart_dec_put(SystemCoreClock);
  uart_msg_put("\r\n");
  uart_msg_put(" Timer ISRs: ");
  uart_dec_put(SwTimerIsrCounter);
  uart_msg_put("\r\n");
}

void display_version() {
  uart_msg_put("\r\nVer: ");
  uart_msg_put( CODE_VERSION );
  uart_msg_put("\r\n");
}

// convert an ADC reading into temperature in C
// See: KL25 reference manual p497
int convert_temp(unsigned int v_temp) {
  return 25 - ((v_temp - V_TEMP25)/M);
}

// ADC display
void display_adcs() {

  uart_msg_put(" ADC CH0: ");
  uart_word_put(adc_vals[0]);

  uart_msg_put("  CH1: ");
  uart_word_put(adc_vals[1]);

  uart_msg_put(" CH2: ");
  uart_word_put(adc_vals[2]);
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
          display_readings();
          display_flag = 0;
        }
      break;
    case(DEBUG):
      // debug menu does not have continuous display
      display_flag = 0;
      break;
    case(ADC):
      if (display_flag == 1) {
        display_adcs();
        display_flag = 0;
      }
      break;
  }
}
