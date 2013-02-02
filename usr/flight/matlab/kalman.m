function [ ret ] = kalman( u, measurement_update, y, N, T_out, R, Q )
%KALMAN Preforms one step of pitch/roll Kalman filter
    % x_hat is the roll/pitch estimate, u is all other state variables
    % N is number of times to run prediction step
    % T_out is output sample rate
    % R is measurement noise covariance (3x3)
    % Q is process noise covariance (2x2)

    persistent x_hat
    persistent P
    
    if isempty(x_hat)
        x_hat = [0; 0];
        P = [0 0; 0 0];
    end
    
    for i = 1:N
        x_hat = x_hat + (T_out/N) * f(x_hat, u);
        A = df_dx(x_hat, u);
        P = P + (T_out/N) * (A*P + P*transpose(A) + Q);
    end
    
    if measurement_update
        C = dh_dx(x_hat, u);
        L = P * transpose(C) * (R + C*P*transpose(C))^(-1);
        P = P * (eye(2) - L*C);
        x_hat = x_hat + L * (y - h(x_hat, u));
    end
    
    ret = x_hat;
end

