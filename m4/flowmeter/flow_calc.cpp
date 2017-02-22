#include "flow_calc.h"
#include "math_funcs.h"  /* alternate to math.h, much smaller */

int temp = 0;
int freq = 0;
int flow = 0;

// convert an ADC reading into temperature in C
// See: KL25 reference manual p497
// v_temp is the adc reading of the internal temperature sensor
int calc_temp(unsigned int v_temp) {
  temp =  25 - ((v_temp - V_TEMP25)/M);
  return temp;
}

// determines the frequency of vortex values sampled from the ADC
int calc_freq(unsigned short * vals, int sample_count) {
  unsigned char lp_win = 2;  // two on either side
  //unsigned int cross_win = 2;
  unsigned int lp[sample_count];

  // low pass
  for( int i=lp_win; i+lp_win < sample_count; i++  ) {
    lp[i] = 0;
    for( int j=-lp_win; j<=lp_win; j++ ) {
      lp[i] += vals[i+j];
    }
    lp[i] = lp[i]/(2*lp_win+1); // not necessary if we scale crossing or center on zero
  }

  // center/zero crossing detector
  unsigned int cross_val = 0x8000;  // adc data is 0-65535, choose center
  int crossings = 0;
  int cur_sign = 0;
  for( int i=lp_win; i+lp_win < sample_count; i++  ) {
    // could change to two stage if we set cur_sign =0 on change, and only update crossing after
    if( (lp[i] > cross_val) && (cur_sign <= 0) ) {
      cur_sign = 1;
      crossings++;
    } else if( (lp[i] < cross_val) && (cur_sign >= 0) ) {
      cur_sign = -1;
      //crossings++;  // positive crossings only
    }
  }

  // time = samples / 10k (100us samples)
  // freq = crossings/time
  freq = (10000 * crossings) / sample_count;
  return freq;
}

// calculates flow rate based on vortex frequency and temperature
// freq in Hz
// temperature in celsius
int calc_flow(int freq, int temp) {
  float T_C = temp;         // temperature in C, approx 73.4 F
  float T_K = T_C + 273.15; //  ...in Kelvin

  float d_in = 0.5;          // bluff body width in inches
  float d_m = d_in * 0.0254; // ...in meters

  float pid_in = 2.9;            // pipe inner diameter (inches)
  float pid_m = pid_in * 0.0254; //  ...in meters

  // (10) dynamic viscosity of water in kg/(m*s) = (Pa*s) = (N*s)/m^2
  // should be ~= 1e-3, 9.321e-4 @ 23 C
  // http://www.viscopedia.com/viscosity-tables/substances/water/
  // Note: pow10 calculates y=10^x, but takes 1000x and returns 1000y 
  float viscosity = 2.4*10e-5 * (pow10((int) 1000*247.8/(T_K-140))/1000.0);

  // (9) density of water in kg/m^3  (should be ~1000)
  float density = 1000 * (1 - (T_C+288.9414)/(508929.2*(T_C+68.12963))*(T_C-3.9863)*(T_C-3.9863));

  // iterate to find solution
  float error = 99999.0;
  float v_m = 10;  // % initail guess
  float v_m_prev; // previous guess
  float Re, St;
  while( error > 0.0001 ) {
    // (8) Reynolds number  (dimensionless: kg/m^3 * m/s * m * (m*s)/kg)
    //     typical for vortex: 10^5 - 10^7
    // NOTE: v_m is a guess
    Re = density * v_m * pid_m / viscosity;

    // (7) Strouhal number (dimensionless)
    //     should be 0.1 - 0.3
    St = 0.2648 - 1.0356 / sqrt((unsigned int) Re);

    v_m_prev = v_m;
    v_m = freq * d_m / St;
    error = v_m - v_m_prev;
    if(error < 0) { error = -error; }
  }

  // velocity in feet/s
  float v_f = v_m * 3.2808399;

  // flow in gpm
  float flow_gpm = 2.45 * pid_in*pid_in * v_f;

  flow = (int) flow_gpm;
  return flow;
}

// fake data for testing: 1000Hz sine wave sampled at 100us => 3203 gpm
unsigned short adc_test_data[VORTEX_INPUT_SIZE] = {
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3,
0x7FFF,
0xCB3B,
0xF9BB,
0xF9BB,
0xCB3B,
0x7FFF,
0x34C3,
0x0643,
0x0643,
0x34C3
};
