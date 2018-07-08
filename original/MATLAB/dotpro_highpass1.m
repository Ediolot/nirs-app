function dotpro_highpass1(fn_wl1,fn_wl2,fn_ga,dat_in)

    oldpath = path;
    restoredefaultpath; addpath(pathdef);
   
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    clear datai;
    clear refdata;
    
    dat = dat_in;
    dat.session = 1;
    dat.Fc = dat_in.Fc; % Hz, frec. cort. paso alto.
   try
    write_wl(fn_wl1,dat);   
    write_wl(fn_wl2,dat); 
   catch e
           display(['Error writing wl file: ' ,e.message]) ;
   end;
    restoredefaultpath; addpath(oldpath);

    
    
   function write_wl(file_wl,dat)
    apply_HF = true;
    [pth,file_out,file_ext] = fileparts(file_wl); 
    config_in = [pth,filesep,file_out,'_config.txt'];
    
    pth = [pth,filesep,'out'];
    config_out = [pth,filesep,file_out,'_config.txt'];
    file_out = [pth,filesep,file_out,file_ext];
 try
    mkdir(pth);
    copyfile(config_in,config_out);
 catch e
    display(['Error writing wl file: ' ,e.message]) ;
    return;
 end;
    wl = load (file_wl);   
    figure ('Name','Wavelenght'), plot(wl(:,floor(end/2))); pause(0.1);
    wl = dotpro_Band_Pass(dat,wl);  
    wl = wl - min(wl(:)); %% evita que haya valores negativos en las long. de onda wl1.    
    dlmwrite(file_out,wl,' ');
    figure ('Name','Filtered Wavelenght'), plot(wl(:,floor(end/2)));  
    
    pause(0.1);
      
    clear wl 