function [ ret ] = h(x, u)
    phi = x(1);
    theta = x(2);

    ret = zeros(3,1);

    ret(1,1) = u.q * u.Va * sin(theta) + u.g * sin(theta);
    ret(2,1) = u.r * u.Va * cos(theta) - u.p * u.Va * sin(theta) - u.g * cos(theta) * sin(phi);
    ret(3,1) = -u.q * u.Va * cos(theta) - u.g * cos(theta) * cos(phi);
end