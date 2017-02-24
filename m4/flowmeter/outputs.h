#ifndef _LED_H
#define _LED_H

#define LED_ON 0
#define LED_OFF 1

#define PWM_SQUARE 0.5  /* 50% duty, square wave */
#define PWM_OFF 0.0

#define HZ_PER_GPM 1
#define HZ_PER_VORTEX 10

// optionally send freq/flow PWM signals to LEDs for debugging
//#define FLOW_TO_LED
//#define FREQ_TO_LED

#define LCD_SPI_LENGTH 8  /* 8-bit word, model specific */
#define LCD_SPI_MODE 0  /* depends on LCD model */
#define LCD_SPI_SPEED 2000000  /* depends on LCD model */

void led_init();
void pwm_init();
void lcd_init();

void lcd_display();

void output_flow_420();    // TMP0 channel 3  proporional rate to flow
void output_freq_pulse();  // TMP0 channel 4  propotional rate to frequency

void red_heartbeat();

#endif
