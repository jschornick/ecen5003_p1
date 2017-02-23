/*-----------------------------------------------------------------------------
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1                                                --
--                Microcontroller Firmware                                   --
--                  main.cpp                                                 --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
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
-- Functional Description:  Main code file generated by mbed, and then
--                          modified to implement a super loop bare metal OS.
--
-- Copyright (c) 2015, 2016 Tim Scherr  All rights reserved.
--
*/

#include "drivers/Ticker.h"
//#include "drivers/DigitalOut.h"
#include "drivers/Serial.h"
//#include "drivers/AnalogIn.h"
using namespace mbed;


// NOTE: may not link if these includes are before mbed includes
#include "timer.h"
#include "monitor.h"
#include "uart.h"
#include "led.h"
#include "flow_calc.h"
#include "adc.h"

Ticker tick;  //  Creates a timer interrupt using mbed methods

int main()
{

  /* startup message  */
  uart_msg_put("\r\nSystem Reset\r\nCode ver. ");
  uart_msg_put( CODE_VERSION );
  uart_msg_put("\r\n");
  uart_msg_put( COPYRIGHT );
  uart_msg_put("\r\n");

  uart_msg_put("Core clock speed: ");
  uart_dec_put(SystemCoreClock);
  uart_msg_put("\r\n\r\n");

  if( adc_init() == CAL_SUCCESS ) {
    uart_msg_put("ADC calibration comoplete.\r\n");
  } else {
    uart_msg_put("ADC calibration failed!\r\n");
  }

  led_init();
  uart_init();  // switch to buffered uart mode
  
  uint32_t count = 0;
  tick.attach(&timer0, T100US_IN_SECS);
  
  display_menu();

  // Cyclical Executive Loop
  while(1)
  {
    count++;  // counts the number of times through the loop

    /****************  ECEN 5003 add code as indicated  ***************/
    uart_poll();  // Polls the serial port
    read_message_from_uart();  // checks for a serial port message received
    read_all_adcs();  // read ADC channels (if flag set)

    //if( display_flag ) {
    calc_temp(adc_vals[2]);
    calc_freq(adc_test_data, VORTEX_INPUT_SIZE);
    calc_flow(freq, temp);
    //}

    monitor();       // Sends serial port output messages depending

    led_420_output();     // use TMP0 channel 3  proporional rate to flow
    led_pulse_output();   // use TMP0 channel 4  propotional rate to frequency

    //LCD_Display()   // use the SPI port to send flow number

    if ( red_heartbeat_flag ) {
      red_heartbeat();
    }
  }

}

