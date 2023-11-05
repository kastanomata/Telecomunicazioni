close all
clear
clc

% Carica il file dati del soggetto 7 task Baseline
data=load('.\02_FilteredData\FilteredData_Subject_7_task_BL.mat');

% Estrarre il segnale tra n=50 e N=2500
n_start = 50;
n_end = 2500;

yn = data.ECG(n_start:n_end);

% Percentuali di campioni da eliminare
percent_inizio = 0.03;  % 3% all'inizio
percent_meta = 0.05;   % 5% a metà

% Calcola quanti campioni da eliminare all'inizio e a metà
num_campioni = length(yn);
num_campioni_inizio = round(percent_inizio * num_campioni);
num_campioni_meta = round(percent_meta * num_campioni);

% Crea il segnale yn eliminando i campioni
yn(1:num_campioni_inizio) = 0;  % Imposta i primi campioni a 0
yn(floor(num_campioni/2)-(num_campioni_meta/2) : floor(num_campioni/2) + (num_campioni_meta/2)) = 0;  % Imposta i campioni a metà a 0

% Calcola il valore medio e energia del segnale yn
mean_value_yn = mean(yn);
energy_yn = sum(yn.^2);

% Grafico del segnale yn
figure;
plot(yn);
title('Grafico del Segnale yn');
xlabel('Tempo(n)');
ylabel('Ampiezza');

% Disegna una linea orizzontale per il valore medio
hold on;
plot([1, n_end], [mean_value_yn, mean_value_yn], 'r--', 'LineWidth', 2);

% Mostra il valore medio e l'energia nel grafico
rectangle('Position', [500, -0.35, 1500, 0.1], 'EdgeColor', 'b', 'LineWidth', 0.5, 'FaceColor', 'w');
text(1150, -0.3, ['Valore Medio: ' num2str(mean_value_yn)], 'FontSize', 9);
text(550, -0.3, ['Energia: ' num2str(energy_yn) 'J'], 'FontSize', 9);
legend('Segnale', 'Valore medio');
