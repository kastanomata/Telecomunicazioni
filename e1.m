close all
clear
clc
 

% Carica il file dati del soggetto 7 task Baseline
data=load('.\02_FilteredData\FilteredData_Subject_7_task_BL.mat');
% Estrarre il segnale tra n=50 e N=2500
n_start = 50;
n_end = 2500;

xn = data.ECG(n_start:n_end);

% Grafico del segnale xn
figure;
plot(xn);
hold on;

% Etichette degli assi e titolo
xlabel('Tempo (n)');
ylabel('Ampiezza');
legend('Segnale');
title('Grafico del Segnale xn');

% Imposta limiti sull'asse delle x
xlim([n_start, n_end]);
