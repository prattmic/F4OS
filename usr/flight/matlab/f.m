function [ ret ] = f(x, u)    
    phi = x(1);
    theta = x(2);

    ret = zeros(2,1);

    ret(1,1) = u.p + u.q * sin(phi) * tan(theta) + u.r * cos(phi) * tan(theta);
    ret(2,1) = u.q * cos(phi) - u.r * sin(theta);
end