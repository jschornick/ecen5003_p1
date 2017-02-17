#include "mbed.h"
#include "MMA8451Q.h"
#include "tsi_sensor.h"

#define ACCEL_I2C_ADDR (0x1d << 1)

// per FRDM-KL25Z schematic:
// ACCEL(4) : SCL <-> KL25(24) : PTE24/TPM0_CH0/I2C0_SCL
// ACCEL(6) : SDA <-> KL25(25) : PTE25/TPM0_CH1/I2C0_SDA
#define ACCEL_SDA PTE25
#define ACCEL_SCL PTE24

// TSIO_CH9  <-> KL25(51) : PTB16/TSI0_CH9/SPI1_MOSI/UART0_RX/TPM_CLKIN0/SPI1_MISO
// TSIO_CH10 <-> KL25(52) : PTB17/TSI0_CH10/SPI1_MISO/UART0_TX/TPM_CLKIN1/SPI1_MOSI
#define TSI_ELEC0 PTB16  // TSIO_CH9
#define TSI_ELEC1 PTB17  // TSIO_CH10
#define TSI_RANGE 40

#define UPDATE_HZ 50  // How often to update the sensors and output

int main(void) {

  MMA8451Q accel(ACCEL_SDA, ACCEL_SCL, ACCEL_I2C_ADDR);
  TSIAnalogSlider tsi(TSI_ELEC0, TSI_ELEC1, TSI_RANGE);
	
  // LEDs are 100% on when PWM duty cycle is 0
  PwmOut red_led(LED_RED);
  PwmOut green_led(LED_GREEN);
  PwmOut blue_led(LED_BLUE);

  float slider;   // slider reading
  float x, y, z;  // accelerometer readings
  float x_prev, y_prev, z_prev;  // previous readings
  float sensitivity = 0.5;    // accelerometer value multiplier, set by slider
  
  // reduce PWM period from default of 20ms, reduces strobing
  green_led.period_ms(1);
  blue_led.period_ms(1);
  red_led.period_ms(1);
  
  while (true) {

    slider = tsi.readPercentage();
    
    // Get acceleration values in G's (2.0G max)
    x = accel.getAccX();
    y = accel.getAccY();
    z = accel.getAccZ();

    if (slider > 0) {
			red_led = 1.0 - slider;
			green_led = 1.0 - slider;
			blue_led = 1.0 - slider;
			printf("Slider: %1.2f\r\n", slider);
      sensitivity = 2.0 * slider;
    } else {
      red_led = 1.01 - sensitivity * abs(x - x_prev);
      green_led = 1.01 - sensitivity * abs(y - y_prev);
      blue_led = 1.01 - sensitivity *  abs(z - z_prev);
      printf("X: %+1.3f, Y: %+1.3f, Z: %+1.3f\r\n", x, y, z);
    }
    
    x_prev = x;
    y_prev = y;
    z_prev = z;
    
    wait(1/UPDATE_HZ);
  }

}

