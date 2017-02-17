#include "mbed.h"
#include "MMA8451Q.h"
#include "tsi_sensor.h"

#define ACCEL_I2C_ADDR (0x1d << 1)

// per FRDM-KL25Z schematic:
// ACCEL(4) SCL <-> PTE24/TPM0_CH0/I2C0_SCL
// ACCEL(6) SDA <-> PTE25/TPM0_CH1/I2C0_SDA
#define ACCEL_SDA PTE25
#define ACCEL_SCL PTE24

#define TSI_ELEC0 PTB16  // TSIO_CH9
#define TSI_ELEC1 PTB17  // TSIO_CH10
#define TSI_RANGE 40

int main(void) {

  MMA8451Q accel(ACCEL_SDA, ACCEL_SCL, ACCEL_I2C_ADDR);
  TSIAnalogSlider tsi(TSI_ELEC0, TSI_ELEC1, TSI_RANGE);
	
  PwmOut red_led(LED_RED);
  PwmOut green_led(LED_GREEN);
  PwmOut blue_led(LED_BLUE);

  float x, y, z, slider;
	
  while (true) {
    x = abs(accel.getAccX());
    y = abs(accel.getAccY());
    z = abs(accel.getAccZ());
    slider = tsi.readPercentage();

    red_led = 1.0f - x;
    green_led = 1.0f - y;
    blue_led = 1.0f - z;

    printf("X: %1.2f, Y: %1.2f, Z: %1.2f, Slider: %1.2f\r\n", x, y, z, slider);
    wait(0.1f);
  }

}

