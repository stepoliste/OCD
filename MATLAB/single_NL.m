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

load("NL_syms_value.mat");

NL = NL_adapt;

Z_dist = diag([Rin, NL_adapt, R12, Z_C8, Z_C9, R_L, Z_C7, R10, R9, R11, R13]);

Fv = [1, 0,-1, 0, 0;
     1, 0, 0, 0, 0;
     -1,-1, 0, 0, 0;
      0,-1,-1, 1, 0;
      0, 0, 0, 0,-1;
      0,-1,-1, 0, 1];

Fi = [1, 0,-1, 0, 0;
     1, 0, 0, 0, 0;
     -1,-1, 0, 0, 0;
      0, 0, 0, 1,-1;
      0, 0, 0, 0,-1;
      0, 0, 0, 0, 0];

Bv_dist = [eye(6), Fv];
Bi_dist = [eye(6), Fi];

S_dist = eye(length(Z_dist)) - 2*Z_dist*Bi_dist'*((Bv_dist*Z_dist*Bi_dist')\Bv_dist);

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

S_out = eye(length(Z_out)) - 2*Z_out*B_out'*((B_out*Z_out*B_out')\B_out);

%% Initialization for the Non-Linearity
load("COMBINED_IV_characteristics.mat");

% Sample I-V
[Vvect, Ivect, Z_mos] = TransDiodeSamplerNegative(IV_comb.voltage', IV_comb.current');

% Kirchoff to Wave conversion
aVect = (Vvect + Z_dist(2,2) * Ivect);
bVect = (Vvect - Z_dist(2,2) * Ivect);

[mu0, mu1, etaVect] = CPWL_param(aVect, bVect, 0); 

%%
plot(Vvect, Ivect);
hold on
plot(IV_comb.voltage, IV_comb.current, 'b--')

%% Algorithm

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
a_init_C1 = 0;
a_init_C4 = 0;
a_init_C6 = 0;

a_init_C7 = 0;
a_init_C8 = 0;
a_init_C9 = 0;

a_init_C10 = 0;
a_init_C11 = 0;

tic;
for n = 1:N
    
    % Input stage
    b_in(1) = Vin(n);
    b_in(4) = a_init_C4;
    b_in(6) = a_init_C1;
    b_in(8) = V_ref;
    b_in(10) = a_init_C6;

    a_in = S_in * b_in;
     
    % Save capacitor states
    a_init_C1 = a_in(6);
    a_init_C4 = a_in(4);
    a_init_C6 = a_in(10);

    % Save output voltage
    V_1(n) = (a_in(5) + b_in(5))/2;

    % Distortion stage
    b_dist(4) = a_init_C8;
    b_dist(5) = a_init_C9;
    b_dist(1) = V_ref;
    b_dist(9) = V_1(n);
    b_dist(7) = a_init_C7;

    % Compute wave at the NL port
    a_dist(2) = S_dist(2,:) * b_dist;
    
    % Compute nonlinear reflected wave at port 2
    [b_dist(2), idx] = CPWL_function(a_dist(2), aVect, mu0, mu1, etaVect);

    % Compute final a_dist
    a_dist = S_dist * b_dist;
        
    % Save capacitor states
    a_init_C9 = a_dist(5);
    a_init_C8 = a_dist(4);
    a_init_C7 = a_dist(7);

    % Save output voltage
    V_2(n) = (a_dist(6) + b_dist(6)) / 2;
    
    % Output stage
    b_out(4) = V_2(n);
    b_out(5) = a_init_C10;
    b_out(7) = a_init_C11;
    
    a_out = S_out * b_out;
    
    % Save capacitor states
    a_init_C10 = a_out(5);
    a_init_C11 = a_out(7);
    
    % Save output voltage
    V_out(n) = (a_out(3) + b_out(3))/2;
    
end
toc_sim = toc;
%% Plot vs Simscape

load("final_output.mat")
figure('Color', 'white')
plot(t, V_out, 'b','LineWidth',2,'DisplayName','WD');
hold on
plot(final_output.Time, final_output.Data, 'r--', 'LineWidth', 2, 'DisplayName','Simscape')
xlabel('Time [s]','interpreter','latex','FontSize',18);
ylabel('Voltage [V]','interpreter','latex','FontSize',18);
legend('show','interpreter','latex','FontSize',13);

%% Performance Metrics

% Simscape signal
Vsim = interp1(final_output.Time, final_output.Data, t, 'linear', 'extrap');
Vsim = Vsim';

% RMS values
rms_WDF = sqrt(mean(V_out.^2));
rms_Simscape = sqrt(mean(Vsim.^2));
rms_error = sqrt(mean((V_out - Vsim).^2));

% SNR
signal_power = mean(V_out.^2);
error_power = mean((Vsim - V_out).^2);
SNR_dB = 10 * log10(signal_power / error_power);

% Peak error and correlation
peak_error = max(abs(V_out - Vsim));
% corr_val = corr(V_out, Vsim);
% Manual Pearson correlation coefficient
corr_val = sum((V_out - mean(V_out)) .* (Vsim - mean(Vsim))) / ...
           (sqrt(sum((V_out - mean(V_out)).^2)) * sqrt(sum((Vsim - mean(Vsim)).^2)));


% Real-time performance
real_time_ratio = StopTime / toc_sim;

% Display
fprintf('\n--- Performance Metrics ---\n');
fprintf('RMS(WDF): %.4f V\n', rms_WDF);
fprintf('RMS(Simscape): %.4f V\n', rms_Simscape);
fprintf('RMS Error: %.4f V\n', rms_error);
fprintf('Peak Error: %.4f V\n', peak_error);
fprintf('Correlation: %.4f\n', corr_val);
fprintf('SNR: %.2f dB\n', SNR_dB);
fprintf('Elapsed Time: %.4f s for %.4f s audio\n', toc_sim, StopTime);
fprintf('Real-Time Ratio: %.2f x\n', real_time_ratio);

%% Save data from simscape simulation

% IV_comb.current = out.curr_comb.Data;
% IV_comb.voltage = out.volt_comb.Data;
% 
% save('COMBINED_IV_characteristics.mat', 'IV_comb');
