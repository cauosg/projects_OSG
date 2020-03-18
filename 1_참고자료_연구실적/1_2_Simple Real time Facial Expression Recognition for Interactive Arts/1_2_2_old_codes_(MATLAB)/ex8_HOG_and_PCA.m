clear all;
close all;
clc;

CS = 25;

f = imread('1.jpg');
g = rgb2gray(f);
g = imresize(g,[100,100]);

[hogFeature, v]=extractHOGFeatures(g, 'CellSize',[CS CS],'BlockOverlap',[0,0]);

plot(v);

%feature nums : numbins * (n/CS)^2
%overlapping