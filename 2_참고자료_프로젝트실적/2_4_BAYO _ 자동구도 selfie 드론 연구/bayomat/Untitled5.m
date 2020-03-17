H1 = lpfilter('gaussian',30,30,8,0);
H2 = lpfilter('gaussian',30,30,10,0);
H = H2 - H1;
csvwrite('cfilters2/c_filter5.csv',H);