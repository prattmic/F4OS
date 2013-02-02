function [ ret ] = dh_dx(x, u)
    phi = x(1);
    theta = x(2);

    ret = zeros(3,2);

    ret(1,1) = 0;
    ret(1,2) = u.q * u.Va * cos(theta) + u.g * cos(theta);
    ret(2,1) = -u.g * cos(phi) * cos(theta);
    ret(2,2) = -u.r * u.Va * sin(theta) - u.p * u.Va * cos(theta) + u.g * sin(phi) * sin(theta);
    ret(3,1) = u.g * sin(theta) * cos(theta);
    ret(3,2) = (u.q * u.Va + u.g * cos(phi)) * sin(theta);
end