function wl =dotpro_High_Pass(dat,wl0)
 d = BoasFilter(dat.Fc);
 wl = filtfilt(d.sosMatrix,d.ScaleValues,wl0);
