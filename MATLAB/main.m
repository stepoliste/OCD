clc;
clearvars;
close all;

%% Input Signal
f0 = 1000;
fs = 96000;
Ts = 1/fs;
StopTime = 1;
t = 0:Ts:StopTime;
Vin = sin(2*pi*f0*t);
N = length(Vin);

%% First Stage

Rin = 1e-6;
R2 = 1e6;
Z_C1 = Ts/(2*22e-9);
R3 = 10e3;
R6 = 470e3;
R5 = 2.2e3;
Z_C4 = Ts/(2 * 100e-9);
Z_C6 = Ts/(2 * 220e-12);
R8 = 18e3;
X2 = 1e6;
R_L = 1e9;

Z_in = diag([Rin, R3, X2, Z_C4, R_L, Z_C1, R2, R6, R5, Z_C6, R8]);

%fundamental loop matrix of the V-net
Bv = [1, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0; 
      0, 1, 0, 0, 0, 1,-1, 1, 0, 0, 0;
      0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1;
      0, 0, 0, 1, 0, 0, 0,-1, 1, 0, 0;
      0, 0, 0, 0, 1, 0, 0,-1, 0,-1, 0];

%fundamental loop matrix of the I-net
Bi = [1, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0; 
      0, 1, 0, 0, 0, 1,-1, 1, 0, 0, 0;
      0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1;
      0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0;
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0];

S_in = eye(11) - 2*Z_in*Bi'*((Bv*Z_in*Bi')\Bv);

%% Output Stage
Z_C10 = Ts/(2*10e-6);
R14 = 22e3;
R15 = 33e3;
Z_C11 = Ts/(2*47e-9);
X4 = 10e3;
X5 = 500e3;

Z_out = diag([R14, X4, X5, Rin, Z_C10, R15, Z_C11]);


B_out = [1, 0, 0, 0, 0,-1, 0;
         0, 1, 0,-1, 1, 1, 1;
         0, 0, 1,-1, 1, 1, 0];

% Open switch
% Z_out = [X4, X5, Rin, Z_C10, R15, Z_C11];
% B_out = [1, 0,-1, 1, 1, 1;
%          0, 1,-1, 1, 1, 0];

S_out = eye(length(Z_out)) - 2*Z_out*B_out'*((B_out*Z_out*B_out')\B_out);
%% Algorithm

% First stage
% Initialization
V_ref = 4.5;

b_in = zeros(11,1);
a_in = zeros(11,1);
V_1 = zeros(N, 1);

b_out = zeros(length(Z_out), 1);
a_out = zeros(length(Z_out), 1);
V_out = zeros(N, 1);

% Load of capacitors 
a_init_C1 = 4.5;
a_init_C4 = 4.5;
a_init_C6 = 0;

a_init_C10 = 0;
a_init_C11 = 0;

for n = 1:N
    
    % Input stage
    b_in(1) = Vin(n);
    b_in(4) = a_init_C4;
    b_in(6) = a_init_C1;
    b_in(8) = V_ref;
    b_in(10) = a_init_C6;

    a_in = S_in * b_in;
     
    a_init_C1 = a_in(6);
    a_init_C4 = a_in(4);
    a_init_C6 = a_in(10);

    V_1(n) = (a_in(5) + b_in(5))/2;

    % Output stage
    b_out(4) = V_1(n);
    b_out(5) = a_init_C10;
    b_out(7) = a_init_C11;
    
    % Open switch
    % b_out(3) = V1;
    % b_out(4) = a_init_C10;
    % b_out(6) = a_init_C11;
    
    a_out = S_out * b_out;

    a_init_C10 = a_out(5);
    a_init_C11 = a_out(7);
    
    V_out(n) = (a_out(3) + b_out(3))/2;
    % Open switch
    % V_out(n) = (a_out(2) + b_out(2))/2;
end

%% Plot

figure('Color', 'white')
plot(t, V_out, 'Color', [0.3010 0.7450 0.9330], 'LineWidth', 2)
xlabel('Time [s]','interpreter','latex','FontSize',18);
ylabel('Voltage [V]','interpreter','latex','FontSize',18);
