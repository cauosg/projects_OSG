fnum = 100;
for i = 2:5
    d0 = 0;
    i0 = 1;
    interv = 1;
    m1 = lpfilter('gaussian',fnum,fnum,d0+interv*(i-1));
    m2 = lpfilter('gaussian',fnum,fnum,d0+interv*i);
    m1 = fftshift(m1);
    m2 = fftshift(m2);
    m = m2 - m1;
    m = fftshift(m);
    csvwrite(strcat('filters/c_filter','0' + i,'.csv'),m);
end