clear all;clc;close all; 

load classifier_1.mat;

runtimes = 100000;

CS = 6;
emotionnum = 7; 
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

buffer = zeros(480,640,3);
%%
faceDetector = vision.CascadeObjectDetector();
%webcam �ʱ�ȭ
webcamlist%��밡��ķ����Ʈ
cam = webcam;
cam.AvailableResolutions
cam.Resolution = '640x480';

figure('Position',[-48 76 1600 1200]);
imh = imshow(buffer,'Border','tight','InitialMagnification','fit','InitialMagnification','fit');
%implay(buffer);
%outimg = zeros(640,480);