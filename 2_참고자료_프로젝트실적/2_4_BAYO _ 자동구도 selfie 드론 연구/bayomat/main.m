clc;clear;close all;
f = im2double(imread('9.jpg'));
[MO NO CO] = size(f);
fs = imresize(f,[20 NaN]);
fg = rgb2gray(fs);
fg = edge(fg);
[M N] = size(fg);
%-----------------------------------------------------------------------------
m_rg = fs(:,:,1)+fs(:,:,2);
m_by = fs(:,:,3)+m_rg/2;
bgfreq = 0.2;
itfreq = 1;
interv = 0.1;
out1 = ones([M N]);
out2 = ones([M N]);
for i = 1:9
f_rg = bandpassfilter2(m_rg,'gaussian',bgfreq + interv*(i-1),bgfreq + interv*i,1);
f_by = bandpassfilter2(m_by,'gaussian',bgfreq + interv*(i-1),bgfreq + interv*i,1);
diffmap = abs(f_rg - f_by);
out1 = diffmap.*out1;
subplot(212);imshow(diffmap);

f_gray = bandpassfilter2(fg,'gaussian',itfreq + interv*(i-1),itfreq + interv*i,1);

imap = abs(f_gray);
out2 = imap.*out2;
end

omax = max(max(out1(:)));out1 = out1/omax;
figure;subplot(131); imshow(out1); title('result of color map')
omax = max(max(out2(:)));out2 = out2/omax;
subplot(132); imshow(out2); title('result of intensity map');
outbw = AT(out2,50,0);
out3 = out1.*outbw;
omax = max(max(out3(:)));out3 = out3/omax;
subplot(133); imshow(out3); title('result of intensity + color map')
%-----------------------------------------------------------------------------
bw1 = im2bw(out1);
%bw = AT(out1/omax,20,0);
bw1 = imresize(bw1,[MO NaN]);
Iprops1=regionprops(bw1,'BoundingBox','Image');

bw2 = im2bw(out2);
%bw2=imadjust(out2/omax,[0.5 1],[0 1]);
bw2 = imresize(bw2,[MO NaN]);
Iprops2=regionprops(bw2,'BoundingBox','Image');

bw3 = im2bw(out3);
bw3 = imresize(bw3,[MO NaN]);
Iprops3=regionprops(bw3,'BoundingBox','Image');

figure;subplot(131);imshow(f);title('saliency of color');
[s1 s2] = size(Iprops1);
for i = 1:s1
hold on; r=rectangle('Position', Iprops1(i).BoundingBox);set(r,'edgecolor','g')
end

subplot(132);imshow(f);title('saliency of intensity');
[s1 s2] = size(Iprops2);
for i = 1:s1
hold on; r=rectangle('Position', Iprops2(i).BoundingBox);set(r,'edgecolor','g')
end

subplot(133);imshow(f);title('saliency of color + intensity');
[s1 s2] = size(Iprops3);
for i = 1:s1
hold on; r=rectangle('Position', Iprops3(i).BoundingBox);set(r,'edgecolor','g')
end
