#include "drivers/DigitalOut.h"
#include "drivers/PwmOut.h"
#include "drivers/SPI.h"
using namespace mbed;
//#include "mbed.h"

#include "outputs.h"
#include "flow_calc.h"
#include "timer.h"

// green led <-> PTB19 / TPM2_CH1
// red led   <-> PTB18 / TPM2_CH0
// blue led  <-> PTD1  / TPM0_CH1

// J10:11 / KL25:22 <-> PTE30 / TPM0_CH3
// J2:13  / KL25:23 <-> PTE31 / TPM0_CH4

DigitalOut red_led(LED_RED);
DigitalOut green_led(LED_GREEN);
DigitalOut blue_led(LED_BLUE);

#ifdef FLOW_TO_LED
PwmOut flow_pwm(LED_GREEN);
#else
PwmOut flow_pwm(PTE30);
#endif


#ifdef FREQ_TO_LED
PwmOut freq_pwm(LED_BLUE);
#else
PwmOut freq_pwm(PTE31);
#endif

// PTC4 : SPI0_PCS0
// PTC5 : SPI0_CLK
// PTC6 : SPI0_MOSI
// PTC7 : SPI0_MISO
SPI lcd_spi(PTC6, PTC7, PTC5);
DigitalOut lcd_spi_cs(PTC4);

void pwm_init() {
  flow_pwm = PWM_OFF;
  freq_pwm = PWM_OFF;
}

void led_init() {
  // note that the FRDM LEDs are pulled high
  red_led = LED_OFF;
  blue_led = LED_OFF;
  green_led = LED_OFF;   
}

void lcd_init() {
  lcd_spi_cs = 1;  // select LCD device
  lcd_spi.format(LCD_SPI_LENGTH, LCD_SPI_MODE);
  lcd_spi.frequency(LCD_SPI_SPEED); 
  lcd_spi_cs = 0;
}

void lcd_display() {
  lcd_spi_cs = 1;  // select lcd for communication
  lcd_spi.write(flow);  // display commands depend on LCD model
  lcd_spi_cs = 0;
}

// flow PWM, proportional to rate of flow
// 1 Hz for every 1 gpm
void output_flow_420(void) {
  if( flow > 0 ){
    flow_pwm = PWM_SQUARE;  // 50% duty, square wave
    flow_pwm.period_us((1000000 / HZ_PER_GPM) / flow);
  } else {
    flow_pwm = 0.0;  // 0% duty, off
  }
}

// freq PWM, pulse rate proportional to vortex frequency
// 10 Hz pulse for 1 Hz vortex
void output_freq_pulse(void) {
  if ( freq > 0 ) {
    freq_pwm = PWM_SQUARE; // 50% duty, square wave
    freq_pwm.period_us((1000000 / HZ_PER_VORTEX) / freq); 
  } else {
    freq_pwm = 0.0; // 0% duty, off
  }
}

void red_heartbeat()
{
  red_led = !red_led;
  red_heartbeat_flag = 0;
}

//void toggle_green_led() {
//  green_led = !green_led;
//}

