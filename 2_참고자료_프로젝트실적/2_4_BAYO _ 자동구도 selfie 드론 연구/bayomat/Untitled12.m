clc;clear;close all;
f = im2double(imread('9.jpg'));
imshow(f);
[MO NO CO] = size(f);
fs = imresize(f,[100 100]);

ktemp = ones([100 100]);
g = rgb2gray(fs);
fr = fs(:,:,1);
fg = fs(:,:,2);
fb = fs(:,:,3);
m_rg = (fr+fg)/2;
m_by = (fb+m_rg)/2

for i=1:5
bg = 2 * (i-1);
ed = 2 * i;
k1 = bp(m_rg,'gaussian',bg,ed,0);
k2 = bp(m_by,'gaussian',bg,ed,0);
fgray = bp(g,'gaussian',bg,ed,0);

fgray = fgray+0.01;
[M N] = size(fgray);

kout = abs(k1 - k2) + fgray;
ns = ones([100 100]);
kmap = ns./fgray;
kout = kout.*kmap;
% kmmax = max(max(kmap));
% kmax = max(max(kout));
% kout = kout/kmax;
% figure;imshow(kout);
ktemp = ktemp.*kout;
end

kout = ktemp;
kmax = max(max(kout));
kout = kout/kmax;
kout = imresize(kout,[500 750]);
figure;imshow(kout);

figure;
kout = im2bw(kout);
imshow(kout);

ke = edge(kout,'canny');

