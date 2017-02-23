//#include "drivers/DigitalOut.h"
//#include "drivers/PwmOut.h"
//using namespace mbed;
#include "mbed.h"

#include "led.h"
#include "flow_calc.h"
#include "timer.h"

// green led : PTB19
// red led   : PTB18
// blue led  : PTD1 / TPM0_CH1

// PTE30 : TPM0_CH3
// PTE31 : TPM0_CH4

DigitalOut red_led(LED_RED, LED_OFF);
PwmOut green_led(LED_GREEN);
PwmOut blue_led(LED_BLUE);
void led_init() {
 
}

// green led, rate proportional to rate of flow
void led_420_output(void) {
  green_led=0.5; // duty cycle
  //green_led.period_ms(10000000/flow);
  green_led.period_ms(500);
}

// blue led, rate proportional to vortex frequency
void led_pulse_output(void) {
  //blue_led = 0.0;
  //blue_led.period_ms(1000000/freq);
}

void red_heartbeat()
{
  red_led = !red_led;
  red_heartbeat_flag = 0;
}

void toggle_green_led() {
  green_led = !green_led;
}

