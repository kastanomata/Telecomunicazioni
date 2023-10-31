close all
clear
clc
 

% Carica il file dati del soggetto 6 task Baseline
data = load('.\02_FilteredData\FilteredData_Subject_6_task_BL.mat');
% Estrarre il segnale tra n=500 e N=2500
n = 500;
N = 2500;

%disp(length(data));

xn = data.ECG(n:N);

% Grafico del segnale xn
figure;
plot(xn);
hold on;

% Etichette degli assi e titolo
xlabel('Tempo (n)');
ylabel('Ampiezza');
title('Grafico del Segnale xn');

% Imposta limiti sull'asse delle x
xlim([n, N]);
