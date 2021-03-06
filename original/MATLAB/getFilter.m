function Hd = getFilter
%GETFILTER Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 8.2 and the Signal Processing Toolbox 6.20.
% Generated on: 27-Jun-2014 13:14:43

% FIR Window Lowpass filter designed using the FIR1 function.

% All frequency values are in Hz.

Fs = 14.2;  % Sampling Frequency

%Fpass = 0.05;             % Passband Frequency
Fpass = 0.01;             % Passband Frequency
Fstop = 0.1;             % Stopband Frequency
Dpass = 0.057501127785;  % Passband Ripple
Dstop = 0.0001;          % Stopband Attenuation
flag  = 'scale';         % Sampling Flag

% Calculate the order from the parameters using KAISERORD.
[N,Wn,BETA,TYPE] = kaiserord([Fpass Fstop]/(Fs/2), [1 0], [Dstop Dpass]);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, Wn, TYPE, kaiser(N+1, BETA), flag);
Hd = dfilt.dffir(b);

% [EOF]
