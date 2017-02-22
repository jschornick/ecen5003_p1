#ifndef _FLOW_CALC_H
#define _FLOW_CALC_H

extern int temp;
extern int freq;
extern int flow;
extern unsigned short adc_test_data[];

int calc_temp(unsigned int v_temp);
int calc_freq(unsigned short *, int);
int calc_flow(int, int);

#define VORTEX_INPUT_SIZE 1000

#define V_BG                    (1000U)     /*! BANDGAP voltage in mV (trim to 1.0V) */
#define V_TEMP25                (716U)      /*! Typical VTEMP25 in mV */
#define M                       (1620U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)

#endif
