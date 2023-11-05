close all
clear
clc

% Esercizio 1
% Carica il file dati del soggetto 7 task Baseline
data=load('.\02_FilteredData\FilteredData_Subject_7_task_BL.mat');

% Estrarre il segnale tra n=50 e N=2500
n_start = 50;
n_end = 2500;

% Creare un vettore Xn con i valori dell'intervallo scelto
xn = data.ECG(n_start:n_end);

% Esercizio 2
% Calcolare il valore medio di xn
mean_value = mean(xn);

% Calcolare la varianza di xn
variance = var(xn);

%Esercizio 3
% Calcolare l'energia di xn
energy = sum(xn.^2);

% Esercizio 4
% Creare un vettore Yn, come Xn con i valori dell'intervallo scelto
yn = data.ECG(n_start:n_end);

% Percentuali di campioni da eliminare da Yn
percent_inizio = 0.03;  % 3% all'inizio
percent_meta = 0.05;   % 5% a metà

% Calcola quanti campioni da eliminare all'inizio e a metà
num_campioni = length(yn);
num_campioni_inizio = round(percent_inizio * num_campioni);
num_campioni_meta = round(percent_meta * num_campioni);

% Modifica il segnale Yn eliminando i campioni
yn(1:num_campioni_inizio) = 0;  % Imposta i primi campioni a 0
yn(floor(num_campioni/2)-(num_campioni_meta/2) : floor(num_campioni/2) + (num_campioni_meta/2)) = 0;  % Imposta i campioni a metà a 0

% Calcola il valore medio e energia del segnale yn
mean_value_yn = mean(yn);
energy_yn = sum(yn.^2);

% Esercizio 5
% Carica il file dati del soggetto 6 task Baseline
dataz = load('.\02_FilteredData\FilteredData_Subject_6_task_BL.mat');

% Estrarre il segnale tra n=500 e N=2500
n_startz = 500;
N_endz = 2500;

% Creare un vettore Zn, con i valori dell'intervallo scelto
zn = dataz.ECG(n_startz:N_endz);

% Calcolo correlazione
Ryz = xcorr(zn,yn);

% Esercizio 6
% Calcola la lunghezza minima tra Yn e Zn
min_length = min(length(yn), length(zn));

% Calcola il coefficiente di correlazione tra le porzioni sovrapposte di zznn e RRyyyy
matrix_correlation = corrcoef(yn(1:min_length),zn(1:min_length));

% Estrarre dalla matrice il coefficiente
coeff_correlation = matrix_correlation(1, 2);




figure('Units','normalized', 'OuterPosition',[0 0 1 1]);

% Grafico del segnale xn
subplot(3,1, 1);
plot(xn);
% Etichette degli assi e titolo
title('Grafico del Segnale xn');
xlabel('Tempo (n)');
ylabel('Ampiezza');
legend('Segnale', 'Valore medio');
hold on;
% Visualizzare varianza ed energia nel grafico di Xn
rectangle('Position', [250, 0.68, 1150, 0.25], 'EdgeColor', 'b', 'LineWidth', 1, 'FaceColor', 'w');
text(1000, 0.82, ['Varianza: ' num2str(variance)], 'FontSize', 10);
text(300, 0.82, ['Energia: ' num2str(energy) 'J'] , 'FontSize', 10);

% Disegnare una linea orizzontale per il valore medio di Xn
plot([1, n_end], [mean_value, mean_value], 'r--', 'LineWidth', 2);

% Grafico del segnale Yn
subplot(3,1,2);
plot(yn);
title('Grafico del Segnale yn');
xlabel('Tempo(n)');
ylabel('Ampiezza');
% Disegna una linea orizzontale per il valore medio di Yn
hold on;
plot([1, n_end], [mean_value_yn, mean_value_yn], 'r--', 'LineWidth', 2);

% Mostra il valore medio e l'energia nel grafico di Yn
rectangle('Position', [250, 0.68, 1150, 0.25], 'EdgeColor', 'b', 'LineWidth', 1, 'FaceColor', 'w');
text(1000, 0.82, ['Valore Medio: ' num2str(mean_value_yn)], 'FontSize', 10);
text(300, 0.82, ['Energia: ' num2str(energy_yn) 'J'], 'FontSize', 10);
legend('Segnale', 'Valore medio');


%grafico terzo
subplot(3,1,3);
plot(Ryz);
title('Correlazione tra Zn e Yn');
xlabel('Lag');
ylabel('Valore di Correlazione');
rectangle('Position', [250, 19.3, 1150, 4], 'EdgeColor', 'b', 'LineWidth', 1, 'FaceColor', 'w');
text(260, 21.5, ['Coefficiente di correlazione: ' num2str(coeff_correlation)] , 'FontSize', 10);

