function wl =nirscam_Band_Pass(dat,wl0)


% All frequency values are in Hz.

%Fs = 1.81;  % Sampling Frequency


% Fc1 = 0.01;  % First Cutoff Frequency
% Fc2 = 0.1;   % Second Cutoff Frequency

Fs = 1.81;  % Sampling Frequency

Fstop1 = dat.Fclow*.5;            % First Stopband Frequency
Fpass1 = dat.Fclow;            % First Passband Frequency
Fpass2 = dat.Fc;             % Second Passband Frequency
Fstop2 = dat.Fc*1.1;           % Second Stopband Frequency
% Dstop1 = 0.001;           % First Stopband Attenuation
% Dpass  = 0.057501127785;  % Passband Ripple
% Dstop2 = 0.0001;          % Second Stopband Attenuation
% dens   = 20;              % Density Factor
% 
% % Calculate the order from the parameters using FIRPMORD.
% [N, Fo, Ao, W] = firpmord([Fstop1 Fpass1 Fpass2 Fstop2]/(Fs/2), [0 1 ...
%                           0], [Dstop1 Dpass Dstop2]);
% 
% % Calculate the coefficients using the FIRPM function.
% b  = firpm(N, Fo, Ao, W, {dens});
% Hd = dfilt.dffir(b);

N = 100;
d = fdesign.bandpass('N,Fst1,Fp1,Fp2,Fst2,C',N,Fstop1,Fpass1,Fpass2,Fstop2,Fs);
d.Stopband1Constrained = true; 
d.Astop1 = 60; % dB
d.Stopband2Constrained = true; 
d.Astop2 = 60; % dB
Hd = design(d,'equiripple'); 

%wl = filtfilt(Hd.Numerator,1,wl0);
% wl = filtfilt(d.sosMatrix,d.ScaleValues,wl0);


% %%%%%Aqui falta dise?ar el filtro paso banda para sacar el codigo.
    




   
