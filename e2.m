close all
clear
clc
 

% Carica il file dati del soggetto 7 task Baseline
data=load('.\02_FilteredData\FilteredData_Subject_7_task_BL.mat');% Estrarre il segnale tra n=50 e N=2500
n_start = 50;
n_end = 2500;

%disp(length(data));

xn = data.ECG(n_start:n_end);

% Calcolare il valore medio di xn
mean_value = mean(xn);

figure;
plot(xn);
hold on;

% Disegnare una linea orizzontale per il valore medio
plot([1, n_end], [mean_value, mean_value], 'r--', 'LineWidth', 2);

% Etichette degli assi e titolo
xlabel('Tempo (n)');
ylabel('Ampiezza');
legend('Segnale', 'Valore medio');
title('Grafico del Segnale xn');

% Imposta limiti sull'asse delle x
xlim([n_start, n_end]);
