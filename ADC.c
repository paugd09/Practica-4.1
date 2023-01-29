/*
 * ADC.c
 *
 *  Created on: 26/01/2023
 *      Author: Paulina Cruz Gudiño

    Usando el modulo 0 y 1 , configurar la tarjeta a la frecuencia asignada, para adquirir 6 señales analógicas 
    a una velocidad de 1 millón de muestras por segundo, por los canales asignados  y guardar los valores en un 
    arreglo para ser enviados con un botones  externos asociado al gpio A a través del protocolo de comunicación 
    asíncrona a una velocidad todo esto usando interrupciones.
    
                                                    PIN - CANAL
    c) 3,5,7,4,2,8 20 MHZ 9600, -sec2, sec3, sec1:  PE1 - AIN2, PE0 -* AIN3, PE5 - AIN8, PD3 - AIN4, PD2 -* AIN5, PD0 -* AIN7
                           pag. 104
    modulo 0 -> sec. 1 -> Interrupción 15
    modulo 1 -> sec. 3 -> Interrupción 51
    modulo 1 -> sec. 2 -> Interrupción 50
    */

#include "lib/include.h"

extern void Configura_Reg_ADC(void)
{
    
    //Pag 352 para inicializar el modulo de reloj del adc RCGCADC
    SYSCTL->RCGCADC = (1<<0)|(1<<1);  //Inicializar  del reloj del adc  (RCGCADC), pag. 352 se habilita el modulo 1 y 0
    
    //Pag 340 los puertos (RGCGPIO) Puertos base habilitación del reloj SE habilitan los puertos correspondientes a los canales del adc pg. 801 para canales y puerto
    //                     F     E      D       C      B     A     
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(1<<0); //Port A -> UART, Port F -> leds, Port E y D para canales 
    
    //Pag 663 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida. Pag 801 para pines correspondientes a canales
    GPIOE_AHB->DIR = (0<<1) | (0<<0) | (0<<5); //PE1 PE0 y PE5
    GPIOD_AHB->DIR = (0<<3) | (0<<2) | (0<<0); //PD3 PD2 y PD0
    
    //(GPIOAFSEL) pag.672 Indicar que esos pines trabajan con función alternativa 
    GPIOE_AHB->AFSEL = (1<<1) | (1<<0) | (1<<5); //PE1 PE0 y PE5
    GPIOD_AHB->AFSEL = (1<<3) | (1<<2) | (1<<0); //PD3 PD2 y PD0
    
    //(GPIODEN) pag.683 desabilita el modo digital de los pines porque son analogicos
    GPIOE_AHB->DEN = (0<<1) | (0<<0) | (0<<5); //PE1 PE0 y PE5
    GPIOD_AHB->DEN = (0<<3) | (0<<2) | (0<<0); //PD3 PD2 y PD0
    
    //Pag 688 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1351, Se indica la función alternativa de los pines
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FFF00); //Poner 0 en los bits de los pines 0, 1, 5
    GPIOD_AHB->PCTL = GPIOD_AHB->PCTL & (0xFFFF00F0); //Poner 0 en los bits de los pines 3, 2, 0 
    
    //(GPIOAMSEL) pag.687 habilitar analogico los pines de los canales
    GPIOE_AHB->AMSEL = (1<<1) | (1<<0) | (1<<5); //PE1 PE0 y PE5
    GPIOD_AHB->AMSEL = (1<<3) | (1<<2) | (1<<0); //PD3 PD2 y PD0 
    
    //Pag 891 El registro (ADCPC) establece la velocidad de conversión por segundo
    ADC0->PC = (0<<3)|(1<<2)|(1<<1)|(1<<0);//1Mms  SE pone 7 en binario porque trabaja con la max. velocidad que es 1 millon
    ADC1->PC = (0<<3)|(1<<2)|(1<<1)|(1<<0);//1Mms  SE pone 7 en binario porque trabaja con la max. velocidad que es 1 millon
    
    //Pag 841 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores 
    ADC0->SSPRI = 0x2103; //prioridad 0 (mayor) -> sec 1,  1 -> sec.2, 2-> sec. 3, 3 (menor) -> sec. 0
    ADC1 ->SSPRI = 0x1023; // prioridad  0 (mayor) -> sec. 2,  1 -> sec. 3, 2 -> sec. 1, 3(menor) -> sec. 0

    //Pag 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0); //SE desactivan los 4 secuenciaores para configurar
    ADC1->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);
    
    //Pag 834 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger) (Si es por procesador o por otro) ***
    ADC0->EMUX  = (0x0000); // Por procesador. *confg. antes:  Sec. 1 se da un valor de 4 para configurar con interrupciones ( Extetrnal GPIO)
    ADC1->EMUX  = (0x0000); // Por procesador. *confg. antes: Sec. 2 y 3 se da un valor de 4 para configura con interrupciones ( Extetrnal GPIO)
   
    //Pag 867 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado, Se establece que canal estará en cada mux por lo tanto en cada secuenciador y en cada modulo
    ADC0->SSMUX1 = 0x0357; // Los canales 3,5, 7 se definen para el mux 1 del sec. 1 en el modulo 0
    ADC1->SSMUX2 = 0x0042; // Los canales 4,2  se definen para el mux 2 del sec. 2 en el modulo 1
    ADC1->SSMUX3 = 0x8;  // El canal 8  se define para el mux 3 del sec. 3 en el modulo 1 

    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción
    //    Sec. 1        mux 0            mux 1             mux 2      son donde se establecen los canales en la configuración del SSMUX1 -> linea 70 
    ADC0->SSCTL1 = (1<<2) | (1<<1)| (1<<6) | (1<<5) | (1<<10) | (1<<9); // Para establecer configuración de muestro y finalizar muestro acorde a la pag. 868
    //    Sec. 2        mux 0            mux 1 
    ADC1->SSCTL2 = (1<<2) | (1<<1)| (1<<6) | (1<<5);
    //    Sec. 3        mux 0  pag. 876
    ADC1->SSCTL3 = (1<<2) | (1<<1);

    // Pag 825 Enable ADC Interrupt - Interrumpe la conversión del secuenciador a usar 
    ADC0->IM |= (1<<1); /* Unmask ADC0 sequence 2 interrupt*/  // Al sec. 1  se le asigna un 1 para indicar que manda la señal pura al controlador de interrupciones
    ADC1->IM |= (1<<2) | (1<<3); // Al sec. 2 y 3 se le asigna un 1 para indicar que manda la señal pura al controlador de interrupciones
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;

    //Pag 821 (ADCACTSS) Este registro controla la activación de los secuenciadores utilizados 
    ADC0->ACTSS = (0<<3) | (0<<2) | (1<<1) | (0<<0); // se habilita el sec. 1
    ADC1->ACTSS = (1<<3) | (1<<2) | (0<<1) | (0<<0); // se habilita el sec. 2 y 3 
    ADC0->PSSI |= (1<<1); //se inicializa el sec. 1  cuando se configura el adc en modo de procesador  ***
    ADC1->PSSI |= (1<<2) | (1<<3); //se inicializa el sec. 2 y 3  cuando se configura el adc en modo de procesador  ***

}

/*
//  Metodo de adquisición del ADC 0 y sec. 1
extern void ADC0_InSeq1(uint16_t *Result)
{
       while((ADC0->RIS&0x02)==0){}; // *** espera a que el convertidor termine  (RIS = registro de edo. muestra el edo de la señal de interrupción sin procesar)
       Result[2] = ADC0->SSFIFO1&0xFFF; //  En el FIF01 se almacenan las muestras, por lo que se lee, pag 860
       Result[1] = ADC0->SSFIFO1&0xFFF;  
       Result[0] = ADC0->SSFIFO1&0xFFF;
       printChar('A');
       ADC0->ISC = 0x0002;  //Conversion finalizada, se limpia para volver a convertir 

}

//  Metodo de adquisición del ADC 1 y sec. 2 
extern void ADC1_InSeq2(uint16_t *Result_2)
{
    
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
      // ADC0->PSSI = 0x00000004; //se habilita el modo de configuración
       while((ADC1->RIS&0x04)==0){}; // espera al convertidor
       Result_2[1] = ADC1->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en la pila2
       Result_2[0] = ADC1->SSFIFO2&0xFFF;
       printChar('B');
       ADC1->ISC = 0x0004;  //Conversion finalizada se limpia para volver a convertir 

}

//  Metodo de adquisición del ADC 1 y sec. 3 
extern void ADC1_InSeq3(uint16_t *Result_3)
{
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
      // ADC0->PSSI = 0x00000004; //se habilita el modo de configuración
       while((ADC1->RIS&0x08)==0){}; // espera al convertidor
       Result_3[0] = ADC1->SSFIFO3&0xFFF; //  Leer  el resultado almacenado en la pila2
       printChar('C');
       ADC1->ISC = 0x0008;  //Conversion finalizada, se limpia para volver a convertir 

}*/
