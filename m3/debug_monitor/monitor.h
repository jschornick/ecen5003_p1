/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  monitor.h                                                --
--                                                                           --
-----------------------------------------------------------------------------*/

#ifndef _MONITOR_H
#define _MONITOR_H

#define CODE_VERSION "2.0 2016/09/29"   /*   YYYY/MM/DD  */
#define COPYRIGHT "Copyright (c) University of Colorado"

enum dmode { QUIET, NORMAL, DEBUG };

extern unsigned char Error_status;          // Variable for debugging use

void monitor(void);
void read_message_from_uart(void);

void process_message(void);
void display_menu(void);
void display_stack(void);
void display_sysinfo(void);
void display_registers(void);
void display_readings(void);
void display_memory(void);
void display_version(void);


#endif
