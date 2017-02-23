#ifndef _LED_H
#define _LED_H

#define LED_ON 0
#define LED_OFF 1

void led_init();
void led_420_output();     // use TMP0 channel 3  proporional rate to flow
void led_pulse_output();   // use TMP0 channel 4  propotional rate to frequency

void red_heartbeat();

#endif
