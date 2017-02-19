/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      monitor.h                                            --
--                                                                           --
-----------------------------------------------------------------------------*/

#ifndef _MONITOR_H
#define _MONITOR_H

#define CODE_VERSION "2.0 2016/09/29"   /*   YYYY/MM/DD  */
#define COPYRIGHT "Copyright (c) University of Colorado"

enum dmode { QUIET, NORMAL, DEBUG, VERSION };

extern unsigned char Error_status;          // Variable for debugging use
extern UCHAR  display_timer;  // 1 second software timer for display
extern UCHAR  display_flag;   // flag between timer interrupt and monitor.c, like
                              // a binary semaphore
extern volatile UCHAR red_heartbeat_flag; /* flag set when heartbeat should toggle */
#define RED_HEARTBEAT_RESET 78  /* 6.4ms * 78 = 0.4992 sec */

void monitor(void);  /* located in module monitor.c */
void display_menu(void);          /* located in module monitor.c */
void read_message_from_uart(void);
void process_message(void);


#endif
