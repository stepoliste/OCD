% close all; clear; clc; % Clearing commands are handled by the environment
load('ssc_output.mat')
fs = 44100; % sampling frequency
Ts = 1/fs;
StopTime = 1;
t = 0:Ts:StopTime;
n_ports = 11;
f0 = 1000;
Vb = 4.5; % Assuming Vb is the DC bias
% Let's define an input signal assuming it's connected in series with Vb at port 10
V_in_sig = 0.5*sin(2*pi*f0*t); % Example input signal (1kHz sine wave)

C1 = 2.2e-2;
C4 = 1e-3;
C6 = 220e-12; % Fixed syntax error here, removed trailing '-'

Z1 = 1e-6;   % Port 1 resistance
Z2 = 1e6;    % Port 2 resistance
Z3 = 500e3;  % Port 3 resistance
Z4 = Ts/(2*C4); % Port 4 resistance (Capacitor C4 adapted, rho=0, using trapezoidal rule)
Z5 = 1e-6;   % Port 5 resistance (Output voltage measured here)
Z6 = Ts/(2*C1); % Port 6 resistance (Capacitor C1 adapted, rho=0, using trapezoidal rule)
Z7 = 18e3;   % Port 7 resistance
Z8 = Ts/(2*C6); % Port 8 resistance (Capacitor C6 adapted, rho=0, using trapezoidal rule)
Z9 = 2.2e3;  % Port 9 resistance
Z10 = 470e3; % Port 10 resistance (Input/Bias port)
Z11 = 1e6;   % Port 11 resistance

Z = diag([Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9,Z10,Z11]);

Vout = zeros(length(t),1);
a = zeros(n_ports,1);
b = zeros(n_ports,1);

% These matrices define the junction topology and nullor connections.
% They are calculated from the circuit's fundamental loop and cut-set matrices
% of the V-network and I-network.
Fv = -[0, 0, 0, 0, 0, -1;
       0, 0, 0, 0, 1, -1;
       0, 1, 1, 0, 0, 0;
       0, 0, 0, 1, -1, 0;
       0, 0, -1, 0, -1, 0]';
Fi = -[ 0, 0, 0, 0, 0, -1;
        1, 0, 0, 0, 1, -1;
        0, 1, 1, 0, 0, 0;
        0, 0, 1, 1, 0, 0;
        0, 0, 0, 0, 0, 0]';
I = eye(6);
Qv = [Fv, I];
Qi = [Fi, I];
Bv = [eye(5), -Fv.'];
Bi = [eye(5), -Fi.'];

rho = 0; %current waves

% Calculate the scattering matrix S using the provided formulas for rho=0
% S = eye(n_ports)-2*Z^rho*lambda*Z^(1-rho); % Original formula
% For rho=0, Z^rho = eye(n_ports), Z^(1-rho) = Z
lambda = Bi.'*inv((Bv*Z*Bi.'))*Bv;
S = eye(n_ports) - 2 * lambda * Z; % Formula (38) from the document with rho = 0


% Time loop for simulation
for k = 1:length(t)

    % --- Set the waves reflected from the elements (incident to the junction) ---
    % Since all ports are adapted and rho=0:
    % For passive elements (ports 1-9, 11), the reflected wave is ideally 0.
    b(1:9) = 0;
    b(11) = 0;

    % For the voltage source at port 10 (Vb + V_in_sig(k)):
    % Assuming an adapted voltage source with rho=0, the wave incident to the junction from the source is V_source.
    % This is a simplification assuming the source directly injects this wave value into the adapted port.
    b(10) = Vb + V_in_sig(k);

    % --- Calculate the waves incident to the elements (reflected by the junction) ---
    % This is the core step using the junction scattering matrix
    a = S * b;

    % --- Calculate Vout at port 5 ---
    % Vout is the voltage at the 5th port, connected to Z5.
    % Using the relation v = Z * (a+b)/2 for rho = 0 (current waves)
    Vout(k) = Z(5,5) * (a(5) + b(5)) / 2;

    % --- Update reflected waves 'b' from elements for the next time step ---
    % In this simplified model with adapted ports, the reflected waves from
    % passive elements are always zero, and the source wave is set by the input signal.
    % So, no update to 'b' based on 'a' is needed here for the next iteration,
    % other than the time-varying input signal for the source at port 10.

end % End of time loop

% Vout vector now contains the calculated voltages at port 5 for each time step

plot_lim = 5/f0; % Limit the plot to just 5 periods of the output signal

% --- Inizio del codice per preparare i dati di Simscape ---

% Assicurati che la variabile di output di Simscape esista nell'area di lavoro
% Sostituisci 'out' con il nome della variabile se è diverso
if exist('out', 'var')
    % Accedi all'elemento desiderato nell'oggetto Dataset
    % Sostituisci 'Vout_Simscape' con il nome esatto del segnale registrato per la tensione di uscita
    try
        simscape_data_element = out.getElement('Vout_Simscape');

        % Estrai i vettori tempo e dati
        simscape_time = simscape_data_element.Time;
        simscape_voltage = simscape_data_element.Data;

        % Crea la matrice gt nel formato [tempo; tensione]
        gt = [simscape_time(:)'; simscape_voltage(:)'];

        fprintf('Dati Simscape estratti con successo nella variabile gt.\n');

    catch exception
        fprintf('Errore nell elemento dei dati di Simscape.\n');
        fprintf('Assicurati che il nome dell sia corretto e che i dati siano stati registrati.\n');
        disp(exception.message);
        % Potresti voler interrompere l'esecuzione qui se i dati di Simscape sono essenziali
        % return;
    end

else
    fprintf('La variabile di output di Simscape (es. ''out'') non è stata trovata nell');
    fprintf('Assicurati di aver eseguito la simulazione Simscape e che il logging sia abilitato.\n');
    % Potresti voler interrompere l'esecuzione qui
    % return;
end

% --- Fine del codice per preparare i dati di Simscape ---

% Ora puoi eseguire la parte di simulazione Wave Digital e la sezione di plot
% nel tuo script MATLAB.