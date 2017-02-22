#ifndef _ADC_H
#define _ADC_H

#define ADC_0                   (0U)
#define CHANNEL_0               (0U)   /* VREFL */
#define CHANNEL_1               (1U)   /* vortex sensor */
#define CHANNEL_2               (2U)   /* temperature sensor */
#define ADCR_VDD                (65535U)    /*! Maximum value when use 16b resolution */
#define V_BG                    (1000U)     /*! BANDGAP voltage in mV (trim to 1.0V) */
#define V_TEMP25                (716U)      /*! Typical VTEMP25 in mV */
#define M                       (1620U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)

#define CAL_SUCCESS  0
#define CAL_FAIL     1

int adc_init(void);
void adc_config(void);
int adc_calibrate(void);
unsigned int adc_read(unsigned int channel);

#endif
