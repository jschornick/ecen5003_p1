// initialize the adc and define 3 channel sources
//
// See: KL25 Reference Manual, chapter 28 (p457)
//
// Definition from rom MKL25Z4.h:
//   #define ADC0_BASE   (0x4003B000u)
//   #define ADC0 ((ADC_Type *)ADC0_BASE)

/* PTB0(43) : ADC0_SE8  : ch 0? : VREFL(19) (channel 11110 only?) */
/* PTB1(44) : ADC0_SE9  : ch 1? : J10_4 (vortex in) */
/* PTB2(45) : ADC0_SE12 : ch 2? : temp sensor? (channel 11010 only?) */

// SC1[AB] reset value : 0x 0000 001F
//    interrupt disable
//    single-ended
//    input channel disabled (11111)

// CFG1 reset value : 0x 0000 0000
//    bus clock, single-ended 8-bit, short-sample, divide=1, normal power

// CFG2 reset value : 0x 0000 0000
//    ADxxa channels, async clock out disabled, normal conversion sequence, longest sample time

// SC2 reset value : 0x 0000 0000
//    software trigger, compare disabled, DMA disabled, default voltage references (V_refh, Vrefl)

// Note: default bus clock speed is 24MHz

// the CMSIS Peripheral Access Layer for our processor
#include "MKL25Z4.h"
#include "adc.h"
#include "timer.h"  // adc_flag

unsigned int adc_vals[3];

int adc_init(void) {
  int cal_status;
  // power on the clock for ADC0
  SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
  adc_config();
  cal_status = adc_calibrate();
  adc_config();  // restore any settings modified during calibration
  return cal_status;
}

// ADC must be powered on to configure!
void adc_config(void) {

  // CFG1: ADC Configuration 1 (p466)
  //---------------------------------
  //   reset value: 0x 0000 0000

  // 0x0 = clock divider = 1 (input clock)
  ADC0->CFG1 |= ADC_CFG1_ADIV(0x0);

  // clock source = bus clock (24Mhz default)
  ADC0->CFG1 |= ADC_CFG1_ADICLK(0x0);

  // 0x3 = 16-bit mode
  ADC0->CFG1 |= ADC_CFG1_MODE(0x3);

  // 0x1 = long sample time
  ADC0->CFG1 |= ADC_CFG1_ADLSMP_MASK;

  // 0x0 = disable low-power
  ADC0->CFG1 &= ~(ADC_CFG1_ADLPC_MASK);

  // CFG2: ADC Configuration 2 (p467)
  //---------------------------------
  //   reset value : 0x 0000 0000

  // we need a sample rate of 100 ksps with a 24MHz bus clock
  //   16-bit has a base conversion rate of 25 ADCK cycles, +2 non-high speec
  //   24Mhz / (25+20+2) = 500ksps, 4x averaging?
  // 0x0 = +20 ADCK cycles/sample
  ADC0->CFG2 |= ADC_CFG2_ADLSTS(0x0);

  // SC1[0,1]: Status and Control 1 (A,B)
  //---------------------------------
  //   reset value: 0x 0000 001F
  // 0x0 = Single-ended conversions
  ADC0->SC1[0] &= ~(ADC_SC1_DIFF_MASK);

  // SC2: Status and Control 2 (p470)
  //---------------------------------
  //   reset value : 0x 0000 0000
  // 0x0 = default voltage references (V_refh, Vrefl)
  ADC0->SC2 |= ADC_SC2_REFSEL(0x0);

  // SC3: Status and Control 3 (p472)
  //---------------------------------
  //   reset value : 0x 0000 0000
  // 0x1 = continuous mode enabled
  ADC0->SC3 |= ADC_SC3_ADCO_MASK;
  // 0x1 = averaging enabled
  ADC0->SC3 |= ADC_SC3_AVGE_MASK;
  // 0x00 = 4 sample averate
  ADC0->SC3 &= ~ADC_SC3_AVGS_MASK;

}

// ADC calibration, required for accurate readings
// returns CAL_SUCCESS (0) when successful
//
// See: KL25 reference p494
int adc_calibrate(void) {

  // maximum averaging
  // 0x1 = averaging enabled
  ADC0->SC3 |= ADC_SC3_AVGE_MASK;
  // 0x3 = 32 sample average
  ADC0->SC3 |= ADC_SC3_AVGS_MASK;

  // <4 Mhz
  // 0x3 = clock divider = 8 (input clock/8 = 3MHz)
  //ADC0->CFG1 |= ADC_CFG1_ADIV(0x3);

  // 0x0 = default voltage references (V_refh, Vrefl)
  //ADC0->SC2 |= ADC_SC2_REFSEL(0x0);

  // ADTRG=0 or calibration will fail
  ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;

  // Initiate calibration, set CAL=1
  ADC0->SC3 |= ADC_SC3_CAL_MASK;

  // Wait until calibration succeeds
  //   CAL=1 until calibration completes
  //while( ADC0->SC3 & ADC_SC3_CAL_MASK ) ;

  // At the end of calibration, COCO=1 (p495)
  while( ADC0->SC1[0] & ADC_SC1_COCO_MASK ) ;

  // success when CALF=0
  int calf = (ADC0->SC3 & ADC_SC3_CALF_MASK) >> ADC_SC3_CALF_SHIFT;

  unsigned short calibration = 0;

  calibration += ADC0->CLP0;
  calibration += ADC0->CLP1;
  calibration += ADC0->CLP2;
  calibration += ADC0->CLP3;
  calibration += ADC0->CLP4;
  calibration += ADC0->CLPS;

  calibration /= 2;
  calibration |= 0x8000;  // set MSB of uint16

  ADC0->PG = calibration;

  calibration = 0;
  calibration += ADC0->CLM0;
  calibration += ADC0->CLM1;
  calibration += ADC0->CLM2;
  calibration += ADC0->CLM3;
  calibration += ADC0->CLM4;
  calibration += ADC0->CLMS;

  calibration /= 2;
  calibration |= 0x8000;  // set MSB of uint16

  ADC0->MG = calibration;

  // this might clear CALF as well...
  ADC0->SC3 &= ~(ADC_SC3_CAL_MASK);

  return calf;

}


/* PTB0(43) : ADC0_SE8  : ch 0? : VREFL(19) (channel 11110 only?) */
/* PTB1(44) : ADC0_SE9  : ch 1? : J10_4 (vortex in) */
/* PTB2(45) : ADC0_SE12 : ch 2? : temp sensor? (channel 11010 only?) */
unsigned int adc_read(unsigned int channel) {

  // ADC read steps:
  //   set channel (clears COCO)
  //   check conversion complete
  //   read data register

  // clear channel mask bits and replace with desired channel
  switch (channel) {
    case CHANNEL_0:
      // vrefl  (NOT ADC0_SE8)
      ADC0->SC1[0] = (ADC0->SC1[0] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(30);
      break;
    case CHANNEL_1:
      // vortex/J10_4 (ADC0_SE9)
      ADC0->SC1[0] = (ADC0->SC1[0] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(9);
      break;
    case CHANNEL_2:
      // temperature (ADC0_SE12)
      ADC0->SC1[0] = (ADC0->SC1[0] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(26);
      break;
    default:
      return (unsigned int) -1;
  }

  // COCO=1 when conversion completes, busy wait until true
  while( !(ADC0->SC1[0] & ADC_SC1_COCO_MASK) ) ;

  // clear COCO
  ADC0->SC1[0] &= ~(ADC_SC1_COCO_MASK);

  return ADC0->R[0]; 

}

void read_all_adcs(void) {
  if(adc_flag) {
    adc_vals[0] = adc_read(CHANNEL_0);
    adc_vals[1] = adc_read(CHANNEL_1);
    adc_vals[2] = adc_read(CHANNEL_2);
    adc_flag = 0;
  }
}
