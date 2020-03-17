function H = lpfilter(type, M, N, D0, n)
[U, V] = dftuv(M, N);

D = sqrt(U.^2 + V.^2);

switch type
    case 'ideal'
        H = double (D <= D0);
        
    case 'btw'
        H = 1./(1+ (D./D0).^(2*n));
        
    case 'gaussian'
        H = exp(-(D.^2)./(2*(D0^2)));
        
    otherwise
        error('Unknown Filter Type');
end
end