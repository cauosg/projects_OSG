clc;clear;close all;
f = im2double(imread('9.jpg'));
imshow(f);title('source f');
[MO NO CO] = size(f);
fs = imresize(f,[100 100]);

ktemp = ones([100 100]);
for i=1:5
bg = 2 * (i-1);
ed = 2 * i;
fr = bp(fs(:,:,1),'gaussian',bg,ed,0,i);
fg = bp(fs(:,:,2),'gaussian',bg,ed,0,0);
fb = bp(fs(:,:,3),'gaussian',bg,ed,0,0);
fgray = fr+fg+fb;
fgray = fgray/3+0.01;
[M N] = size(fgray);

k1 = (fg + fr)/2;
k2 = (fb + k1/2)/2;

kout = abs(0.375 * fr + 0.375*fg - 0.5*fb) + fgray;
ns = ones([100 100]);
kmap = ns./fgray;
kout = kout.*kmap;
kmmax = max(max(kmap));
kmax = max(max(kout));
kout = kout/kmax;
ktemp = ktemp.*kout;
kshow = imresize(ktemp,[360 NaN]);
figure;imshow(kshow);title(strcat('saliency of iteration','0' + i));
end

kout = ktemp;
kmax = max(max(kout));
kout = kout/kmax;
kout = imresize(kout,[500 750]);
figure;imshow(kout);title('saliency map');

figure;
kout = im2bw(kout);
imshow(kout);title('saliency region');

ke = edge(kout,'canny');

