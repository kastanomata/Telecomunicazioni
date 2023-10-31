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
Ryz = xcorr(yn,zn);

% Calcola la lunghezza minima tra Yn e Zn
min_length = min(length(yn), length(zn));

% Calcola il coefficiente di correlazione tra le porzioni sovrapposte di zznn e RRyyyy
matrix_correlation = corrcoef(yn(1:min_length),zn(1:min_length));

coeff_correlation = matrix_correlation(1, 2);

disp(coeff_correlation);



% Grafico della correlazione
figure;
plot(Ryz);
hold on
title('Correlazione tra Yn e Zn');
xlabel('Lag');
ylabel('Valore di Correlazione');

rectangle('Position', [220, 20, 2750, 3], 'EdgeColor', 'r', 'LineWidth', 0.5, 'FaceColor', 'w');
text(260, 21.5, ['Coefficiente di correlazione: ' num2str(coeff_correlation)] , 'FontSize', 10);
