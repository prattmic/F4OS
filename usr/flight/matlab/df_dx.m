function [ ret ] = df_dx(x, u)
    phi = x(1);
    theta = x(2);

    ret = zeros(2,2);

    ret(1,1) = u.p * cos(phi) * tan(theta) - u.r * sin(phi) * tan(theta);
    ret(1,2) = (u.q * sin(phi) - u.r * cos(phi))/(cos(theta)^2);
    ret(2,1) = -u.q * sin(phi) - u.r * cos(phi);
    ret(2,2) = 0;
end