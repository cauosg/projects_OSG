clc;clear;close all;
%% read video
v = VideoReader('vids/test2.webm');

%[m,n] = [v.Height,v.Width];
%procsize = [720 1280];
%procsize = [100 100];
runtimes = 1000;
g_thresh = 0.1;
downexp = 4;
procsize = round([v.Height v.Width]/2^downexp);
sigma = 0.3;
sensitivity = 50;
w = [2 50 0.14285];
w1 = gpuArray(im2double(ones(procsize)))* w(1);

%% create c*
%c_star_compo = [165 81 69];
c_star_compo = [182 128 113];
c_star_r = gpuArray(ones(procsize)*c_star_compo(1));
c_star_g = gpuArray(ones(procsize)*c_star_compo(2));
c_star_b = gpuArray(ones(procsize)*c_star_compo(3));
c_star = gpuArray((cat(3,c_star_r,c_star_g,c_star_b))/255);

opp_c = bgr_to_oppo(c_star);
%% preproc


f0 = gpuArray(im2double(readFrame(v)));
for fcounter = 1:runtimes

%% faster resize
f = f0;
f = imresize(f0,0.5^downexp,'cubic');


%% bgr to oppo.m
% o1 = f(:,:,1).*sqrt(1.000/2.000) - f(:,:,2).*sqrt(1.000/2.000);
% o2 = f(:,:,1).*sqrt(1.000/6.000) + f(:,:,2).*sqrt(1.000/6.000) - f(:,:,3).*2*sqrt(1.000/6.000);
% o3 = f(:,:,1).*sqrt(1.000/3.000) + f(:,:,2).*sqrt(1.000/3.000) + f(:,:,3).*sqrt(1.000/3.000);
% out = cat(3,o1,o2,o3);
%%end
opp_f = bgr_to_oppo(f);

%% exp

sal_1 = w(3) * gpuArray(exp(w1 - w(2) * ((opp_f(:,:,1) - opp_c(:,:,1))).^2));
sal_2 = w(3) * gpuArray(exp(w1 - w(2) * ((opp_f(:,:,2) - opp_c(:,:,2))).^2));
sal_3 = w(3) * gpuArray(exp(w1 - w(2) * ((opp_f(:,:,3) - opp_c(:,:,3))).^2));

sim = sal_1.*sal_2.*sal_3;
%% get ROI
tic
t1 = sim<g_thresh ;
t2 = sim<g_thresh ;

timg = (t1+t2)<1;
toc

%% after proc
% timg = imgaussfilt(timg,sigma);
%% 
timg_up = imresize(im2double(timg),2^downexp,'cubic');
f_out = f0.* timg_up;

%% 

imshow(f_out);title(strcat('resized 2^',num2str(downexp),' (',num2str(procsize(2)),' x ',num2str(procsize(1)),')  total:',num2str(procsize(1)*procsize(2)), 'pixels'))
end