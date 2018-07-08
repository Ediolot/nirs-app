function [width, height, bitpix, dark, gain, timestamps, data] = loadExperimentFile(filename)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
fid = fopen(filename);
% Tamaño de la imagen
width = fread(fid, 1, '*int32');
height = fread(fid, 1, '*int32');
% Bits por pixel
bitpix = fread(fid, 1, '*int32');
% Dark Frame
dark = fread(fid, width * height, '*float32');
dark = reshape(dark, width, height);
% Gain Frame
gain = fread(fid, width * height, '*float32');
gain = reshape(gain, width, height);
% Calculo del numero de frames
sizeOfChunk = width * height * 2 + 8;
curr = ftell(fid);
fInfo = dir(filename);
sizeOfFile = fInfo.bytes;
nFrames = (sizeOfFile - curr) / sizeOfChunk;
% Lectura de los timestamps
timestamps = fread(fid, nFrames, 'int64=>float64', width * height * 2);
% Lectura de los frames
fseek(fid, curr + 8, 'bof');
data = fread(fid, nFrames * height * width, [num2str(width * height) '*int16=>int16'], 8);
data = reshape(data, width, height, nFrames);

fclose(fid);

end

