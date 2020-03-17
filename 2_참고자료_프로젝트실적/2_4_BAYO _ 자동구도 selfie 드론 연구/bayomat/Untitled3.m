clc;clear;close all;
f = im2double(imread('21.jpg'));
[MO NO CO] = size(f);
fg = rgb2gray(f);
fg = imresize(fg,[360 NaN]);
tic
fr2 = bp(fg,'gaussian',0.01,20,1);
toc
figure;
imshow(fr2);
fre = edge(fr2,'canny');
figure; imshow(fre);