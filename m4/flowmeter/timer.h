/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  timer.h                                                  --
--                                                                           --
-----------------------------------------------------------------------------*/

#ifndef _TIMER_H
#define _TIMER_H

#define T100US_IN_SECS 0.0001  /* 100 microseconds in seconds */

typedef unsigned char UCHAR;
typedef unsigned char bit;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

extern volatile uint32_t SwTimerIsrCounter;

extern volatile UCHAR red_heartbeat_flag; /* flag set when heartbeat should toggle */
#define RED_HEARTBEAT_RESET 78  /* 6.4ms * 78 = 0.4992 sec */

extern UCHAR display_timer;  // 1.6 seconds when reset to 0
extern UCHAR display_flag;   // set when timer expires, cleared by monitor after output

extern UCHAR  adc_flag;  // flag which times ADC sampling using the timer
                         // interrupt semaphore to main
void timer0(void);

#endif
