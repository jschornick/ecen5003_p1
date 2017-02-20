% data file is 1000 samples with sample period 100us
ascii_hex = cell2mat(textread ('data_1000Hz_1105gpm.txt' , '%s'));
data = hex2dec(ascii_hex)';

n = length(data);

Ts = 100 * 10^-6;  % sample period of 100us (also timer0 tick)
Fs= 1/Ts;          % sampling frequency

voltage_max = 3.3;
adc_max = 2^16-1;

%signal = voltage_max * data / adc_max;
signal = data;

t = [0:n-1] * Ts;  % time axis in seconds
subplot(2,1,1);
plot(1000*t, signal);
title('Signal');
xlabel('time (ms)');
%ylabel('voltage');
ylabel('ADC reading');

dft = fft(signal);

% FFT calculation is symmetric about zero, take first half
mag = abs( dft(1:n/2+1) );

% frequencies are from 0 - Fs/2 (Nyquist freq)
F = linspace(0,Fs/2,length(mag));

subplot(2,1,2);
plot(F,mag);
xlabel('freq (Hz)');
ylabel('power');

% get the frequency with the highest magnitude, ignoring dc component
[max_mag,i] = max([0, mag(2:end)]);

fundamental = F(i)

%%%%%%%%

f = fundamental;  % shedding frequency in Hz
T_C = 23   % temperature in C, approx 73.4 F 
T_K = T_C + 273.15  % temperature in Kelvin

d_in = 0.5   % bluff body width in inches
d_m = d_in * 0.0254   ...in meters

pid_in = 2.9   % pipe inner diameter (inches)
pid_m = pid_in * 0.0254  %  ...in meters

% (10) dynamic viscosity of water in kg/(m*s) = (Pa*s) = (N*s)/m^2
% should be ~= 1e-3, 9.321e-4 @ 23 C
% http://www.viscopedia.com/viscosity-tables/substances/water/
viscosity = 2.4*10^-5 * 10^(247.8/(T_K-140))

% (9) density of water in kg/m^3  (should be ~1000)
density = 1000 * (1 - (T_C+288.9414)/(508929.2*(T_C+68.12963))*(T_C-3.9863)^2)

%%%% iterate to find solution

error = Inf;
v_m = 10;  % initail guess
while( error > 0.0001 ) 

  % (8) Reynolds number  (dimensionless: kg/m^3 * m/s * m * (m*s)/kg)
  %      typical for vortex: 10^5 - 10^7
  % NOTE: v_m is a guess
  Re = density * v_m * pid_m / viscosity
  
  % (7) Strouhal number  (dimensionless)
  %      should be 0.1 - 0.3
  St = 0.2684 - 1.0356 / sqrt(Re)

  v_m_prev = v_m;
  v_m = f * d_m / St
  error = abs( v_m - v_m_prev );
end

% velocity in feet/s
v_f = v_m * 3.2808399

% flow in gpm  (should be 1105)
%    1105 gpm  -->  v_f=53.629 ft/s  (16.346 m/s)
%
% why is this off by a factor of pid_in ??
flow_gpm = 2.45 * pid_in^2 * v_f

%      1 ft^3 = 7.48052 gallons
%      1 ft^3/s = 448.83 gpm
%flow_ft3_s =  (pi*(pid_in/2)^2)/144 * v_f
%flow_gpm = flow_ft3_s * 7.48052*60



