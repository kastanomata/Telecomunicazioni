close all
clear
clc
 

% Carica il file dati del soggetto 6 task Baseline
dataz = load('.\02_FilteredData\FilteredData_Subject_6_task_BL.mat');
datay = load('.\02_FilteredData\FilteredData_Subject_7_task_BL.mat');
% Estrarre il segnale tra n=500 e N=2500
n_startz = 500;
N_endz = 2500;

% Estrarre il segnale tra n=50 e N=2500
n_starty = 50;
N_endy = 2500;

zn = dataz.ECG(n_startz:N_endz);
yn = datay.ECG(n_starty:N_endy);

% Percentuali di campioni da eliminare
percent_inizio = 0.03;  % 3% all'inizio
percent_meta = 0.05;   % 5% a metà

% Calcola quanti campioni da eliminare all'inizio e a metà
num_campioni = length(yn);
num_campioni_inizio = round(percent_inizio * num_campioni);
num_campioni_meta = round(percent_meta * num_campioni);

% Crea il segnale yn eliminando i campioni
yn(1:num_campioni_inizio) = 0;  % Imposta i primi campioni a 0
yn(floor(num_campioni/2)+1 : floor(num_campioni/2) + num_campioni_meta) = 0;  % Imposta i campioni a metà a 0

% Calcolo correlazione
Ryz = xcorr(zn,yn);

% Grafico della correlazione
figure;
plot(Ryz);
hold on 
%plot(zn);
%plot(yn);
title('Correlazione tra Zn e Yn');
xlabel('Lag');
ylabel('Valore di Correlazione');
%legend('Ryz', 'zn', 'yn');
% Imposta limiti sull'asse delle x
xlim([min(n_starty,n_startz), max(N_endz,N_endy)]);


% % Calcola il valore medio e energia del segnale yn
% mean_value_yn = mean(yn);
% energy_yn = sum(yn.^2);
