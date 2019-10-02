function f = gauss_distribution(n, x, mu, c)
    p1 = sqrt( (2*pi).^n * det(c) );
    p2 = exp( - 0.5 * transpose(x - mu) * inv(c) * (x - mu) );
    f = p2./p1; 
