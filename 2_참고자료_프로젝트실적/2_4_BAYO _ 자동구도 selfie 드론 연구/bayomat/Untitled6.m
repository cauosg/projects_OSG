fnums = 360;
    m1 = lpfilter('gaussian',fnums,fnums,d0+interv*(i-1));
    m2 = lpfilter('gaussian',fnums,fnums,d0+interv*i);
    m1 = fftshift(m1);
    m2 = fftshift(m2);
    m = m2 - m1;
    m = fftshift(m);
    csvwrite(strcat('filters/c_filter','0' + i,'.csv'),m);
