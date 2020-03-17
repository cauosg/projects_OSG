function out = bandpassfilter2(image,type,freq1,freq2,order)
image_d = im2double(image);


f = fft2(image_d);
FS = fftshift(f);

SA = abs(FS);
mn = min(min(SA));
SA_1 = SA - mn;
mx = max(max(SA_1));
SA_1 = double(SA_1/mx);

[x, y, c] = size(FS);
lpmask1 = lpfilter(type, x, y, freq1, order);

lpmask1 = fftshift(lpmask1);

lpmask2 = lpfilter(type, x, y, freq2, order);

lpmask2 = fftshift(lpmask2);

lpmask = lpmask2 - lpmask1;
FSA_M = FS.* lpmask;

SA_M = SA_1.* lpmask;

FSA_M = ifftshift(FSA_M);
image_out = real(ifft2(FSA_M));

mn = min(min(image_out));
image_out1 = image_out - mn;
mx = max(max(image_out1));
image_out2 = image_out1 / mx;

out = image_out2;
end
