clc
clearvars;
close all;

%% Input Signal
f0 = 1000;
fs = 96000;
Ts = 1/fs;
StopTime = 1;
t = 0:Ts:StopTime-Ts;
Vin = sin(2*pi*f0*t);
N = length(Vin);

%% First Stage

Rin = 1e-6;
R2 = 1e6;
Z_C1 = Ts/(2 * 22e-9);
R3 = 10e3;
R6 = 470e3;
R5 = 2.2e3;
Z_C4 = Ts/(2 * 100e-9);
Z_C6 = Ts/(2 * 220e-12);
R8 = 18e3;
X2 = 500e3;%1e6;
R_L = 1e6;

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

%% Distortion Stage

R9 = 10e3; 
R10 = 10e3;
Z_C7 = Ts/(2 * 10e-9);
R12 = 220e3;
Z_C8 = Ts/(2 * 100e-9);
Z_C9 = Ts/(2 * 220e-12);
R11 = 39e3;
R13 = 150e3;
D1 = 1e3;
M1 = 1;
M2 = 1;

R_joke = 1e3;

Z_dist = diag([M1, R12, D1, Z_C9, Z_C8, R_L, Rin, R9, Z_C7, R10, M2, R11, R13]);
% Z_dist = diag([R_joke, R12, R_joke, Z_C9, Z_C8, R_L, Rin, R9, Z_C7, R10, R_joke, R11, R13]);

Fv = [0, 1, 0, 0, 0, 0;
      0,1,1, 0, 0, 0;
      0,-1, 0, 1, 0, 0;
      0, 0, 0, 0, 0,-1;
     -1, 0,-1, 0, 1, 0;
     -1, 0,-1, 0, 0, 1;
     -1, 1, 0, 0, 0, 0];

Fi = [0, 1, 0, 0, 0, 0;
      0,1,1, 0, 0, 0;
      0,-1, 0, 1, 0, 0;
      0, 0, 0, 0, 0,-1;
      0, 0, 0, 0, 1,-1;
      0, 0, 0, 0, 0, 0;
     -1, 1, 0, 0, 0, 0];

Bv_dist = [eye(7), Fv];
Bi_dist = [eye(7), Fi];

S_dist = eye(13) - 2*Z_dist*Bi_dist'*((Bv_dist*Z_dist*Bi_dist')\Bv_dist);

%% Output Stage
Z_C10 = Ts/(2 * 10e-6);
R14 = 22e3;
R15 = 33e3;
Z_C11 = Ts/(2 * 47e-9);
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

%% Initialization for mosfets and diode
load("MOSFET_IV_characteristics.mat");

% Sample I-V
% [Vvect, Ivect, Z_mos] = TransDiodeSampler(FET_curr.Time', FET_curr.Data');
% [Vvect_d, Ivect_d, Z_d] = TransDiodeSampler(Diode_curr.Time', Diode_curr.Data');

[Vvect, Ivect, Z_mos] = TransDiodeSampler(IV_data.FET.voltage', IV_data.FET.current');
[Vvect_d, Ivect_d, Z_d] = TransDiodeSampler(IV_data.D.voltage', IV_data.D.current');

% Kirchoff to Wave conversion
aVect_m1 = (Vvect + Z_dist(1,1) * Ivect);
bVect_m1 = (Vvect - Z_dist(1,1) * Ivect);

aVect_m2 = (Vvect + Z_dist(11,11) * Ivect);
bVect_m2 = (Vvect - Z_dist(11,11) * Ivect);

aVect_d = (Vvect_d + Z_dist(3,3) * Ivect_d);
bVect_d = (Vvect_d - Z_dist(3,3) * Ivect_d);

[mu0_m1, mu1_m1, etaVect_m1] = CPWL_param(aVect_m1, bVect_m1, 0); 
[mu0_m2, mu1_m2, etaVect_m2] = CPWL_param(aVect_m2, bVect_m2, 0);
[mu0_d, mu1_d, etaVect_d] = CPWL_param(aVect_d, bVect_d, 0);

%%
plot(Vvect, Ivect);
% hold on
% plot(Vvect_d, Ivect_d, 'b')

%% Algorithm

% First stage

% Initialization
V_ref = 4.5;

b_in = zeros(11,1);
a_in = zeros(11,1);
V_1 = zeros(N, 1);

b_dist = zeros(length(Z_dist), 1);
a_dist = zeros(length(Z_dist), 1);
V_2 = zeros(N, 1);

b_out = zeros(length(Z_out), 1);
a_out = zeros(length(Z_out), 1);
V_out = zeros(N, 1);

% Load of capacitors 
a_init_C1 = 0;%4.5;
a_init_C4 = 0;
a_init_C6 = 0;%4.47;

a_init_C7 = 0;%4.5;
a_init_C8 = 0;%-4.5;%0.2;%0;
a_init_C9 = 0;%4.5;

a_init_C10 = 0; %3.66;
a_init_C11 = 0;%-3.3e-4;

v_old_iter = 0;
eSIM = 1e-3;

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

    % Distortion stage
    b_dist(4) = a_init_C9;
    b_dist(5) = a_init_C8;
    b_dist(7) = V_ref;
    b_dist(8) = V_1(n);
    b_dist(9) = a_init_C7;

    % Nonlinearties
    flag = 1;
    S_temp = S_dist;

    while flag

        % (Nonlinear) Local Scattering Stage

        [b_dist(1), idx1]= CPWL_function(a_dist(1), aVect_m1, mu0_m1, mu1_m1, etaVect_m1);
        [b_dist(11), idx2]= CPWL_function(a_dist(11), aVect_m2, mu0_m2, mu1_m2, etaVect_m2);
        [b_dist(3), idx3]= CPWL_function(a_dist(3), aVect_d, mu0_d, mu1_d, etaVect_d);

        % Global Scattering Stage 

        a_dist = S_temp*b_dist; 

        % Convergence Check

        v = (a_dist + b_dist)/2;

        if max(abs(v - v_old_iter)) < eSIM
            % currents must be computed before the port resistance updates

            % update of port resistances
            Z_dist(1, 1) = Z_mos(idx1);
            Z_dist(11, 11) = Z_mos(idx2);
            Z_dist(3, 3) = Z_d(idx3);

            aVect_m1 = (Vvect + Z_dist(1,1) * Ivect);
            bVect_m1 = (Vvect - Z_dist(1,1) * Ivect);

            aVect_m2 = (Vvect + Z_dist(11,11) * Ivect);
            bVect_m2 = (Vvect - Z_dist(11,11) * Ivect);

            aVect_d = (Vvect_d + Z_dist(3,3) * Ivect_d);
            bVect_d = (Vvect_d - Z_dist(3,3) * Ivect_d);

            [mu0_m1, mu1_m1, etaVect_m1] = CPWL_param(aVect_m1, bVect_m1, 0); 
            [mu0_m2, mu1_m2, etaVect_m2] = CPWL_param(aVect_m2, bVect_m2, 0);
            [mu0_d, mu1_d, etaVect_d] = CPWL_param(aVect_d, bVect_d, 0);

            flag = 0;     
        end  

        v_old_iter = v;
    end

    a_dist = S_dist * b_dist;

    a_init_C9 = a_dist(4);
    a_init_C8 = a_dist(5);
    a_init_C7 = a_dist(9);

    S_dist = eye(13) - 2*Z_dist*Bi_dist'*((Bv_dist*Z_dist*Bi_dist')\Bv_dist);

    V_2(n) = (a_dist(6) + b_dist(6))/2; %v(6);

    % Output stage
    b_out(4) = V_2(n);
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
%% Plot vs Simscape

load("final_output.mat")
figure('Color', 'white')
plot(t, V_out, 'b','LineWidth',2,'DisplayName','WD');
hold on
plot(final_output.Time, final_output.Data, 'r--', 'LineWidth', 2, 'DisplayName','Simscape')
xlabel('Time [s]','interpreter','latex','FontSize',18);
ylabel('Voltage [V]','interpreter','latex','FontSize',18);
legend('show','interpreter','latex','FontSize',13);

%% Save data from simscape simulation
% 
% IV_data.FET.current = out.curr.Data;
% IV_data.FET.voltage = out.volt.Data;
% IV_data.D.current = out.dcurr.Data;
% IV_data.D.voltage= out.dvolt.Data;
% 
% save('MOSFET_IV_characteristics.mat', 'IV_data');
