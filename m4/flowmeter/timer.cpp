/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  Timer.cpp                                                --
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
   Functional Description:
   This file contains code for the only interrupt routine, based on the System
   Timer.
   The System Timer interrupt happens every
   100 us as determined by mbed Component Configuration.
   The System Timer interrupt acts as the real time scheduler for the firmware.
   Each time the interrupt occurs, different tasks are done based on critical
   timing requirement for each task.
   There are 256 timer states (an 8-bit counter that rolls over) so the
   period of the scheduler is 25.6 ms.  However, some tasks are executed every
   other time (the 200 us group) and some every 4th time (the 400 us group) and
   so on.  Some high priority tasks are executed every time.  The code for the
   tasks is divided up into the groups which define how often the task is
   executed.  The structure of the code is shown below:

   I.  Entry and timer state calculation
   II. 100 us group
      A.  Fast Software timers
      B.  Read Sensors
      C.  Update
   III. 200 us group
      A.
      B.
   IV.  400 us group
      A.  Medium Software timers
      B.
    V.   800 us group
      A.  Set 420 PWM Period
   VI   1.6 ms group
      A. Display timer and flag
      B. Heartbeat/ LED outputs
   VII  3.2 ms group
      A. Slow Software Timers
    VIII 6.4 ms group A
       A. Very Slow Software Timers
   IX.  Long time group
      A. Determine Mode
      B. Heartbeat/ LED outputs
   X.  Exit

-- Copyright (c) 2015 Tim Scherr  All rights reserved.
*/

#include "timer.h"

/*********************/
/*   Definitions     */
/*********************/

UCHAR display_timer = 0;  // 1 second software timer for display
UCHAR display_flag = 0;   // flag between timer interrupt and monitor

UCHAR adc_flag = 0;  // 100us per ADC read

UCHAR red_heartbeat_timer = 0;
volatile UCHAR red_heartbeat_flag = 0;

volatile uint32_t SwTimerIsrCounter = 0U;

static uint16_t timer0_count = 0; // 16 bits, counts for 6.5 seconds at 100 us period
static UCHAR timer_state = 0;
//  variable which splits timer_states into groups
//  tasks are run in their assigned group times
//    DigitalOut BugMe (PTB9);   // debugging information out on PTB9


/*********************************/
/*     Start of Code             */
/*********************************/
// I. Entry and Timer State Calculation

void timer0(void)
{

  //  BugMe = 1;  // debugging signal high during Timer0 interrupt on PTB9

  /************************************************/
  //  Determine Timer0 state and task groups
  /************************************************/
  timer_state++;          // increment timer_state each time

  /*******************************************************************/
  /*      100 us Group                                               */
  /*******************************************************************/
  //  II.  100 us Group

  //     A. Update Fast Software timers

  //    B.   Update Sensors
  /****************  ECEN 5003 add code as indicated *****************/
  adc_flag = 1;   // time to sample the ADC in main

  /*******************************************************************/
  /*      200 us Group                                               */
  /*******************************************************************/

  if ((timer_state & 0x01) != 0)  // 2 ms group, odds only
  {
    ;
  } // end  2 ms group

  /*******************************************************************/
  /*      400 us Group                                               */
  /*******************************************************************/
  else if ((timer_state & 0x02) != 0)
  {
    //   IV.  400 us group
    //           timer states 2,6,10,14,18,22,...254

    //      A.  Medium Software timers
    //      B.
  } // end 400 us group

  /*******************************************************************/
  /*      800 us Group                                               */
  /*******************************************************************/
  else if ((timer_state & 0x04) != 0)
  {
    //   V.   8 ms group
    //           timer states 4, 12, 20, 28 ... 252   every 1/8

    //     A.  Set
  } // end 800 us group

  /*******************************************************************/
  /*      1.6 ms Group                                               */
  /*******************************************************************/
  else if ((timer_state & 0x08) != 0)
  {
    // VI   1.6 ms group
    //           timer states 8, 24, 40, 56, .... 248  every 1/16

  } // end 1.6 ms group

/*******************************************************************/
/*      3.2 ms Group                                               */
/*******************************************************************/
  else if ((timer_state & 0x10) != 0)
  {
    // VII  3.2 ms group
    //          timer states 16, 48, 80, 112, 144, 176, 208, 240

    //    A. Slow Software Timers
    //    B.  Update
  } // end 3.2 ms group

/*******************************************************************/
/*      6.4 ms Group A                                             */
/*******************************************************************/
  else if ((timer_state & 0x20) != 0)
  {
    // VIII 6.4 ms group A
    //           timer states 32, 96, 160, 224

    //    A. Very Slow Software Timers

      //    B.  Update

   } // end 6.4 ms group A

/*******************************************************************/
/*      6.4 ms Group B                                             */
/*******************************************************************/
   else
   {
     // IX.  6.4 ms group B
     //       timer states 0, 64, 128, 192

     //    A.  Update

     //    A. Display timer and flag
     display_timer--; // decrement display timer every 6.4 ms.  Total time is
                      // 256*6.4ms = 1.6384 seconds.
     if (display_timer == 1)
       display_flag = 1;     // every 1.6384 seconds, now OK to display

     //    B. Heartbeat/ LED outputs
     //   Generate Outputs  ************************************

     // *** ECEN 5003 add code as indicated ***
     // Create an 0.5 second RED LED heartbeat here.

     // Timer runs down from RED_HEARTBEAT_RESET (78), for a total of 0.4992
     // seconds between heartbeat toggles. Flag is reset after toggle.
     red_heartbeat_timer--;
     if(red_heartbeat_timer == 0) {
       red_heartbeat_flag = 1;  // indicate that we should toggle the heartbeat
       red_heartbeat_timer = RED_HEARTBEAT_RESET;
     }

   }   // end 6.4 ms group B

  /*******************************************************************/
  /*      Long Time Group                                            */
  /*******************************************************************/
  // Re-enable interrupts and return
  timer0_count++;
  SwTimerIsrCounter++;
  //   Bugme = 0;  // debugging signal high during Timer0 interrupt on PTB9

}

