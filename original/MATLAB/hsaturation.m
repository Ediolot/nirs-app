function [satImage] = hsaturation(image1, image2)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    %LAMBDA1 = 852;
    CEM_HBO2_LAMBDA1 = 1062;
    CEM_HB_LAMBDA1 = 691.08;
    %LAMBDA2 = 630;
    CEM_HBO2_LAMBDA2 = 610;
    CEM_HB_LAMBDA2 = 5148.8;

    logImage1 = log(image1);
    logImage2 = log(image2);
    logImage = logImage1 ./ logImage2;
    tmpImage1 = (logImage * CEM_HB_LAMBDA2) - CEM_HB_LAMBDA1;
    tmpImage2 = (logImage * (CEM_HB_LAMBDA2 - CEM_HBO2_LAMBDA2)) + (CEM_HBO2_LAMBDA1 - CEM_HB_LAMBDA1);
    satImage = tmpImage1 ./ tmpImage2;
end

