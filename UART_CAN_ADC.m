%Paulina Cruz Gudiño

% Programa para graficar los valores de los 6 canales del ADC enviados por 
% UART de la tiva hacia matlab 


clc
clear all
close all

serial = serial('com8',9600);
muestras = 10000; %Cantidad de muestras que se quieren
i = 1;

%Formato de la gr�fica
figure('Name', 'Se�ales de 6 canales del ADC', 'NumberTitle', 'on','color',[1 1 1]); grid on; hold on;
title('Voltajes de 6 canales del ADC', 'FontName', 'Arial', 'FontSize', 12);
xlabel('time [s]', 'FontName', 'Arial', 'FontSize', 9);
ylabel('Amplitud', 'FontName', 'Arial', 'FontSize', 9);

%Limintes de la gr�fica
xlim([0 100]);
ylim([0 4]);

while i<muestras
    %Para modificar limite en eje x y hacer una grafica movible 
    if mod(i,100) == 0 %Si i es multiplo de 100
       xlim([i i+100]);
    end
    
    %Para guardar valores
    valor1 = readline(serial); 
    
    %Convierte valor1(caracteres) a valores numericos
    Canal_7 = str2double(valor1);
    
    %Hacer conversi�n
    voltaje_7(i) = (Canal_7*3.3)/4096;
    
    %Graficar
    plot(voltaje_7, 'm');
    drawnow;
    
    i = i+1;  
end


