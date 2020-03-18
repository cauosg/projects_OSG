fsize = 100;
for i = 1:9
    d0 = 0.2;
    i0 = 1;
    interv = 0.1;
    m1 = lpfilter('gaussian',20,20,d0+interv*(i-1));
    m2 = lpfilter('gaussian',20,20,d0+interv*i);
    m1 = fftshift(m1);
    m2 = fftshift(m2);
    m = m2 - m1;
    m = fftshift(m);
    csvwrite(strcat('filters/c_filter','0' + i,'.csv'),m);
end