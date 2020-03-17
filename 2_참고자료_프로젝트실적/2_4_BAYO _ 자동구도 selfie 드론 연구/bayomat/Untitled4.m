clc;clear;close all;
f = im2double(imread('21.jpg'));
[MO NO CO] = size(f);
fg = rgb2gray(f);

fr1 = imresize(fg,[20 NaN]);

%fr2 = bp(fg,'gaussian',0.01,10,1);

fr2 = imresize(fr1,[MO NaN]);

figure;
imshow(fr2);
fre = edge(fr2,'canny');
figure; imshow(fre);