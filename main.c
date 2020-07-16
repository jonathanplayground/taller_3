/*
 * File:   main.c 
 * Author: Jonathtan Guerrero
 *
 * Descripcion
 * Solucion Taller 3
 * Created on 14 de julio de 2020, 01:21 PM
 */

// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 1000000  //CRISTAL 1Mhz

uint16_t  contador = 998;
static int display;

uint8_t mil;
uint8_t cent;
uint8_t dec;
uint8_t uni;
uint16_t numero;
int tiempo = 10;
int pwm = 0;

    
void __interrupt() tcInt(void){//     only     process timer0-triggered interrupts
   
   
   if(PIR1bits.ADIF  && PIE1bits.ADIE) 
      {
       PIR1bits.ADIF = 0;
       
       if(ADRESH < 184)
       {
           pwm  =   1;
       }
       if(pwm & ADRESH > 225)
       {
           pwm = 0;
           contador++;
           eeprom_write (0x20, contador);
       }
   }
}

void descomponerNumero(numero)
{
    mil      = numero /1000;
    numero   = numero % 1000;
    cent     = numero /100;
    numero   = numero % 100;
    dec      = numero /10;
    uni      = numero % 10;
}
void main(void) {
    TRISA   =   0x01;
    TRISB   =   1;
    TRISC   =   0;
    TRISD   =   0;
    ADCON0  =   0X81;
    ADCON1  =   0X05;       //resultado justificado a la derecha, utilizando entrada AN0  y Vref para seleccionar el valor de voltaje maximo
    OPTION_REG  =   0X40;
    INTCON  =   0XC0;
    PIR1bits.ADIF   =   0;
    PIE1bits.ADIE   =   1;
    //contador = eeprom_read(0x20);     //separar en dos registros
    
//    PORTC = 0;
    
    while(1)    
    {
//-------------------------------------------------------------------------------
//configuracion señal pwm
        
        ADCON0bits.GO    =   1;     //preguntar funcionamiento
        while(ADCON0bits.GO);

//---------------------------------------------------------------------------------
//configuracion maquina estados
        
        switch(display)
        {
            case 0:
                descomponerNumero(contador);
                PORTD = 0X07;
                PORTC = uni;
                __delay_ms(tiempo);
                display = 1;
                break;
                
            case 1:
                PORTD = 0x0B;
                PORTC = dec;
                __delay_ms(tiempo);
                display = 2;
                break;
            
            case 2:
                PORTD = 0x0D;
                PORTC = cent;
                __delay_ms(tiempo);
                display = 3;
                break;
                
            case 3:
                PORTD = 0x0E;
                PORTC = mil;
                __delay_ms(tiempo);
                display = 0;
                break;
                
        }
    }   
    
}
