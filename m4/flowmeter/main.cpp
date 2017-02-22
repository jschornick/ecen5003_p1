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

#include "timer.h"
#include "monitor.h"
#include "uart.h"

#include "drivers/Ticker.h"
#include "drivers/DigitalOut.h"
#include "drivers/Serial.h"
#include "drivers/AnalogIn.h"
using namespace mbed;

//#include "mbed.h"

// won't compile if we include this before mbed files??
#include "flow_calc.h"
#include "adc.h"

#define LED_ON 0
#define LED_OFF 1

Ticker tick;  //  Creates a timer interrupt using mbed methods

/****************  ECEN 5003 add code as indicated  ***************/
// Add code to control red, green and blue LEDs here
DigitalOut green_led(LED_GREEN, LED_OFF);
DigitalOut red_led(LED_RED, LED_OFF);
DigitalOut blue_led(LED_BLUE, LED_OFF);

Serial pc(USBTX, USBRX);

void red_heartbeat()
{
  red_led = !red_led;
  red_heartbeat_flag = 0;
}

void toggle_green_led() {
  green_led = !green_led;
}


int main()
{

  pc.printf("Core clock: %d\r\n", SystemCoreClock);

  if( adc_init() == CAL_SUCCESS ) {
    pc.printf("ADC calibration success!\r\n");
  } else {
    pc.printf("ADC calibration failed!\r\n");
  }

  /****************  ECEN 5003 add code as indicated  ***************/
  //  Add code to call timer0 function every 100 uS
  tick.attach(&timer0, T100US_IN_SECS);

  uint32_t  count = 0;

  uart_init();

  /* startup message  */
  uart_direct_msg_put("\r\nSystem Reset\r\nCode ver. ");
  uart_direct_msg_put( CODE_VERSION );
  uart_direct_msg_put("\r\n");
  uart_direct_msg_put( COPYRIGHT );
  uart_direct_msg_put("\r\n");

  display_menu();

  // Cyclical Executive Loop
  while(1)
  {
    count++;  // counts the number of times through the loop

    /****************  ECEN 5003 add code as indicated  ***************/
    uart_poll();  // Polls the serial port
    read_message_from_uart();  // checks for a serial port message received
    read_all_adcs();  // read ADC channels (if flag set)

    if( display_flag ) {
    calc_temp(adc_vals[2]);
    calc_freq(adc_test_data, VORTEX_INPUT_SIZE);
    calc_flow(freq, temp);
    }
      
    monitor();       // Sends serial port output messages depending

    //4-20 output ()    // use TMP0 channel 3  proporional rate to flow

    //Pulse output()   // use TMP0 channel 4  propotional rate to frequency

    //LCD_Display()   // use the SPI port to send flow number

    //if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)
    //{
      //toggle_green_led();
    //}
    if ( red_heartbeat_flag ) {
      red_heartbeat();
    }
  }

}

