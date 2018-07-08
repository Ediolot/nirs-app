function analisis (width, height, bitpix, dark, gain, timestamps, data) 

[width, height, bitpix, dark, gain, timestamps, data] = loadExperimentFile('experiment_2.exp');
time = (timestamps - timestamps(1)) / 1000000000.0;
% calculo del basal
selection = time < 24; % el basal termina a los 24 segundos
start_selection = 1;
end_selection = find(selection, 1, 'last');
    % promediado de las imagenes
tmp = sum(data(:,:,1:end_selection), 3);

tmp = tmp / ((end_selection - start_selection) + 1);
    % correccion flatfield
tmp = (tmp - dark) .* gain;
    % calculo de saturación
im1 = tmp(1:width/2, :);
im2 = tmp((width/2)+1:width, :);
basal = hsaturation(im1, im2);
 imshow(basal);

% muestra las imagenes de rampa en secuencia
selection = (24 < time) & (time < 287);
start_selection = find(selection, 1, 'first');
end_selection = find(selection, 1, 'last');


img1Mean = [];
img2Mean = [];
for i = start_selection:end_selection
    tmp = cast(data(:,:,i), 'double');
    tmp = (tmp - dark) .* gain;
    im1 = tmp(1:width/2, :);
    
    im2 = tmp((width/2)+1:width, :);
    mask = (im1 > 0.9) | (im2 > 0.9);
    im1(mask) = 0;
    im2(mask) = 0;
    img1Mean(i - start_selection + 1) = mean(im1(:));
    img2Mean(i - start_selection + 1) = mean(im2(:));
    currSatImage = hsaturation(im1, im2);
    tmpImg = currSatImage - basal; 
    imshow(tmpImg * 1000, jet);
    pause(0.1);
end


figure1;
hold on;
plot(img1Mean);
plot(img2Mean);
hold off;
