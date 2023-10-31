close all
clear
clc
 

% Carica il file dati del soggetto 7 task Baseline
data=load('.\FilteredData_Subject_7_task_BL.mat');
% Estrarre il segnale tra n=50 e N=2500
n_start = 50;
n_end = 2500;

%disp(length(data));

xn = data.ECG(n_start:n_end);

% Calcolare il valore medio di xn
mean_value = mean(xn);

% Calcolare la varianza di xn
variance = var(xn);

% Calcolare l'energia di xn
energy = sum(xn.^2);

% Grafico del segnale xn
figure;
plot(xn);
hold on;

% Disegnare una linea orizzontale per il valore medio
plot([1, n_end], [mean_value, mean_value], 'r--', 'LineWidth', 2);

% Visualizzare varianza ed energia nel grafico
rectangle('Position', [500, -0.35, 1500, 0.1], 'EdgeColor', 'b', 'LineWidth', 0.5, 'FaceColor', 'w');
text(1300, -0.3, ['Varianza: ' num2str(variance)], 'FontSize', 10);
text(550, -0.3, ['Energia: ' num2str(energy) 'J'] , 'FontSize', 10);


% Etichette degli assi e titolo
xlabel('Tempo (n)');
ylabel('Ampiezza');
legend('Segnale', 'Valore medio');
title('Grafico del Segnale xn');

% Imposta limiti sull'asse delle x
xlim([n_start, n_end]);
