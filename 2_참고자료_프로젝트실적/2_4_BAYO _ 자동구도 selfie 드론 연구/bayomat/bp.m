function out = bp(image,type,freq1,freq2,order,roshow)
f = fft2(image);
FS = fftshift(f);

SA = abs(FS);
mn = min(min(SA));
SA_1 = SA - mn;
mx = max(max(SA_1));
SA_1 = double(SA_1/mx);
if (roshow==1)
figure;
subplot(221);
imshow(SA_1.^0.15);title('image frequency');
end
[x, y, c] = size(FS);
if (freq1>0)
lpmask1 = lpfilter(type, x, y, freq1, order);

lpmask1 = fftshift(lpmask1);

lpmask2 = lpfilter(type, x, y, freq2, order);

lpmask2 = fftshift(lpmask2);

lpmask = lpmask2 - lpmask1;
else
    lpmask = lpfilter(type, x, y, freq2, order);
    lpmask = fftshift(lpmask);
end
if (roshow==1)
subplot(222);
imshow(lpmask);title('lpmask');
end
 FSA_M = FS.* lpmask;

 SA_M = SA_1.* lpmask;
 if (roshow==1)
subplot(223)
imshow(im2uint8(SA_M.^0.15));title('filtered frequency');
 end
 FSA_M = ifftshift(FSA_M);
 image_out = real(ifft2(FSA_M));
% 
 mn = min(min(image_out));
 image_out1 = image_out - mn;
 mx = max(max(image_out1));
 image_out2 = image_out1 / mx;

if (roshow==1)
subplot(224);
imshow(image_out2);title('filtered image');
end
 out = image_out2;

end
