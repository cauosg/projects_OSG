clc;clear;close all;
f = im2double(imread('21.jpg'));
[MO NO CO] = size(f);
fg = rgb2gray(f);
fg = imresize(fg,[360 640]);
f_rg = bp(fg,'gaussian',0.01,10,0);

frst = fg - f_rg;

mn = min(min(frst));
fr1 = frst - mn;
mx = max(max(fr1));
fr2 = fr1 / mx;
figure;
imshow(f_rg);
figure;
imshow(fr2);
fre = edge(fr2,'canny');

figure; imshow(fre);