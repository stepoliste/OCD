close all; clear; clc;

% %%
% Ts = 96000;
% 
% Rin = 1e-6;
% RL = 1e6;
% R9 = 10e3; 
% R10 = 10e3;
% Z_C7 = Ts/(2 * 10e-9);
% R12 = 220e3;
% Z_C8 = Ts/(2 * 100e-9);
% Z_C9 = Ts/(2 * 220e-12);
% R11 = 39e3;
% R13 = 150e3;
% syms NL;
% 
% 
% Z_dist = diag([Rin, NL, R12, Z_C8, Z_C9, RL, Z_C7, R10, R9, R11, R13]);
% 
% Fv = [1, 0,-1, 0, 0;
%      -1, 0, 0, 0, 0;
%      -1,-1, 0, 0, 0;
%       0,-1,-1, 1, 0;
%       0, 0, 0, 0,-1;
%       0,-1,-1, 0, 1];
% 
% Fi = [1, 0,-1, 0, 0;
%      -1, 0, 0, 0, 0;
%      -1,-1, 0, 0, 0;
%       0, 0, 0, 1,-1;
%       0, 0, 0, 0,-1;
%       0, 0, 0, 0, 0];
% 
% Bv = [eye(6), Fv];
% Bi = [eye(6), Fi];
% 
% S = eye(length(Z_dist)) - 2*Z_dist*Bi'*((Bv*Z_dist*Bi')\Bv);
% 
% NL_adapt = solve(S(2,2)==0, NL);
% 
% NL_adapt = double(NL_adapt);

%% Distortion Stage
Ts = 1/96000;
Rin = 1e-6;
R_L = 1e6;

R9 = 10e3; 
R10 = 10e3;
Z_C7 = Ts/(2 * 10e-9);
R12 = 220e3;
Z_C8 = Ts/(2 * 100e-9);
Z_C9 = Ts/(2 * 220e-12);
R11 = 39e3;
R13 = 150e3;

syms NL;

Z_dist = diag([Rin, NL, R12, Z_C8, Z_C9, R_L, Z_C7, R10, R9, R11, R13]);

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

NL_adapt = solve(S_dist(2,2)==0, NL);
NL_adapt = double(NL_adapt);

%% 
save("NL_syms_value", "NL_adapt");
