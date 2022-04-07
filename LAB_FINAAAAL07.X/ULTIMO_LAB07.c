/* 
 * File:   ULTIMO_LAB07.c
 * Author: ALBA RODAS
 *
 * Created on 4 de abril de 2022, 16:55 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project ecounters instead of #define for ON and OFF.

/*------------------------------------------------------------------------------
 * LIBRERIAS 
 ------------------------------------------------------------------------------*/
#include <xc.h>
#include <stdio.h> 

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
// BANCO DE VARIABLES PARA: CONTADOR DE BOTONES, EN PORTA.

uint8_t counter;                    // VARIABLE PARA CONTADOR
uint8_t states_transistores;        // VARIABLE PARA ESTADOS DE TRANSITORES (3 TRANSISTORES)

// BANCO DE VARIABLES PARA: GUARDAR VALORES 'UNIDAD', 'DECENA', 'CENTENA'

uint8_t unidades_variable;
uint8_t decena_variable;
uint8_t centena_variable;                  // VARIABLES PARA: UNIDADES, DECENAS, CENTENAS.


// BANCO DE VARIABLES PARA: GUARDAR VALORES PARA MOSTRARLA EN SU RESPECTIVO DISPLAY (DESPUES DE PASAR POR TABLA)

uint8_t unidades_display;
uint8_t decenas_display;  
uint8_t centenas_display;              

/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void config_ins_outs(void);
void config_timer0(void);
void config_clk(void);
void config_pullups(void);
void config_interrupciones(void);
void divisor
(
uint8_t counter,                        
uint8_t *centena, 
uint8_t *decena, 
uint8_t *unidad
);

// ¿Porque usar el *?
// ?*? this is the universal selector of CSS. As the name says Universal Selector, 
// it selects each and every element present inside your file.

uint8_t tabla(uint8_t value_traduccion);
/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void)
{
    if(INTCONbits.T0IF)         // SI LA BANDERA DEL TMR0 ESTÁ ENCENDIDA, SE CUMPLE CONDICION Y LIMPIO BANDERA + PORTD
    {
        INTCONbits.T0IF = 0;    // CLR BANDERA DEL TMR0
        PORTD = 0;              // CLR AL TRANSISTOR QUE ESTÉ ENCENDIDO 
        
        switch(states_transistores)
        {
        case 0:                             // CASE PARA ENCENDER EL TRANSISTOR CORRESPONDIENTE A CENTENAS
            states_transistores = 1;
            PORTD = 0b001;                  // TRANSISTOR NO.1 == ON
            PORTC = centenas_display;       // PORTC, TOMA EL VALOR DEL DATO QUE HAYA EN 'centenas_display'
            TMR0 = 131;                     // PARA EL 'DURANTE EL LAB' SOLO SE PEDIA UN DELAY DE 100ms para mostrar funcionamiento.
            break;
        
        case 1:                             // CASE PARA ENCENDER EL TRANSISTOR CORRESPONDIENTE A DECENAS
            states_transistores = 2;
            PORTD = 0b010;                  // TRANSISTOR NO.2 == ON
            PORTC = decenas_display;        // PORTC, TOMA EL VALOR DEL DATO QUE HAYA EN 'decenas_display'
            TMR0 = 131;                     // PARA EL 'DURANTE EL LAB' SOLO SE PEDIA UN DELAY DE 100ms para mostrar funcionamiento.
            
            break;
            
        case 2:                             // CASE PARA ENCENDER EL TRANSISTOR CORRESPONDIENTE A UNIDADES
            states_transistores = 0;
            PORTD = 0b100;                  // CASE PARA ENCENDER EL TRANSISTOR CORRESPONDIENTE A UNIDADES
            PORTC = unidades_display;       // TRANSISTOR NO.3 == ON
            TMR0 = 131;                     // PARA EL 'DURANTE EL LAB' SOLO SE PEDIA UN DELAY DE 100ms para mostrar funcionamiento.
            break;
            
        default:
            states_transistores = 0;
            TMR0 = 131;                     // PARA EL 'DURANTE EL LAB' SOLO SE PEDIA UN DELAY DE 100ms para mostrar funcionamiento.
            break;                          // TMR0 FUNCIONANDO A 2ms.
        }
    }  
}
/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) 
{
    config_ins_outs();                      //
    config_clk();
    config_timer0();
    config_interrupciones();
    config_pullups();
    
    while(1)
    {
        if(!PORTBbits.RB1)      // CONDICIÓN: SOLO SI EL BOTON 'RB0 = 0' ENTRO AL 'WHILE'.
        {
            while(!RB1);        // SI EL BOTON 1 'RB0' = 0, ENTONCES DECREMENTO '--' EL CONTADOR DEL PORTA Y PORTC.
            counter --;
        }
        
        if(!PORTBbits.RB0)      // CONDICIÓN: SOLO SI EL BOTON 'RB1 = 0' ENTRO AL 'WHILE'.
        {
            while(!RB0);        // SI EL BOTON 1 'RB1' = 0, ENTONCES INCREMENTO '++' EL CONTADOR DEL PORTA Y PORTC.
            counter ++;
        }
        
        PORTA = counter;        // MUESTRO LO QUE ESTÁ EN EL CONTADOR 'counter' en el PORTA.
        
        // DEFINIMOS LA DIVISION PARA PODER MOSTRAR LA INFORMACION EN EL CONTADOR DE DISPLAYS, COMO LO HACIAMOS EN ASSEMBLER.
        
        // PASO 1: DIVIDIMOS POR DIGITOS, EL VALOR QUE SE ENCUENTRE EN EL CONTADOR.
        divisor(counter, &centena_variable, &decena_variable, &unidades_variable);
        
        // HACEMOS LA TRADUCCIÓN DE BINARIO --> DECIMAL, USANDO LA TABLA DE VALORES.
        
        unidades_display = tabla(unidades_variable);            // LE DOY UN NUEVO VALOR A 'unidades_display' = VALOR DE UNIDADES TRADUCIDO POR TABLA.
        decenas_display = tabla(decena_variable);               // LE DOY UN NUEVO VALOR A 'decenas_display' = VALOR DE DECENAS TRADUCIDO POR TABLA.
        centenas_display = tabla(centena_variable);             // LE DOY UN NUEVO VALOR A 'centenas_display' = VALOR DE CENTENAS TRADUCIDO POR TABLA.
        
        PORTC = centenas_display;
        PORTD = 0b1;
    }
}

/*------------------------------------------------------------------------------
 * CONFIGURACIONES
 ------------------------------------------------------------------------------*/

void config_ins_outs(void)        
{
    ANSEL   =   0;              // DEFINO QUE NECESITO SALIDAS DIGITALES.
    ANSELH  =   0;              // DEFINO QUE NECESITO SALIDAS DIGITALES.
    
    // SALIDA , TRIS = 0
    TRISA   =   0;              // PORTA = SALIDA PARA LEDs.
    TRISB   =   0b11;           // PORTB = RB0 y RB1, SALIDAS PARA BOTONES DE INCREMENTO Y DECREMENTO.
    TRISC   =   0;              // PORTC = SALIDAS PARA DISPLAYS DE 7-SEG.
    TRISD   =   0b11111000;     // PORTD = SALIDAS PARA TRANSISTORES.
    
    // LIMPIO PUERTOS
    PORTA   =   0;              // CLR AL PORTA.
    PORTB   =   0;              // CLR AL PORTB.
    PORTC   =   0;              // CLR AL PORTC.
    PORTD   =   0;              // CLR AL PORTD.
    return;
}

void config_clk(void)           // CONFIGURO LA FRECUENCIA DEL OSCILADOR
{
    OSCCONbits.IRCF = 0b100;    // ESTABLEZCO EL OSCILADOR A: 1MHz
    OSCCONbits.SCS = 1;         // ACTIVO EL RELOJ INTERNO.
    return;
}

void config_timer0(void)        // CONFIGURO EL TIMER0
{
    OPTION_REGbits.T0CS = 0;    // ELIJO USAR EL RELOJ INTERNO DEL PIC.
    OPTION_REGbits.T0SE = 0;    
    OPTION_REGbits.PSA = 0;     // ESTABLEZCO UN PRESCALER AL TMR0
    OPTION_REGbits.PS2 = 0;     // DEBO UTILIZAR UN PRESCALER DE: (001 = 1:4)
    OPTION_REGbits.PS1 = 0;     
    OPTION_REGbits.PS0 = 1;     
    TMR0 = 131;                 // OBTENGO UN VALOR DE '131' PARA UN DELAY DE 2ms.
    return;
}

void config_pullups(void)       // CONFIGURO LOS PULLUPS INTERNOS DEL PORTB
{
    OPTION_REGbits.nRBPU = 0;   // ACTIVO LOS WEAK PULLUPS.
    WPUB = 0b11;                // ACTIVO LOS PULLUPS DEL RB0 Y RB1
    return;
}

void config_interrupciones(void)           // CONFIGURO LAS INTERRUPCIONES
{
    INTCONbits.GIE  = 1;        // ACTIVO LAS INTERRUPCIONES GLOBALES, PARA PODER UTILIZAR INTERRUPCIONES.
    INTCONbits.T0IE = 1;        // ACTIVO INTERRUPCIONES PARA EL TMR0.
    INTCONbits.T0IF = 0;        // APAGO LA BANDERA DEL TMR0.
    return;
}

/*------------------------------------------------------------------------------
 * FUNCIONES (DEFINICION)
 ------------------------------------------------------------------------------*/
// DIVISION:

void divisor(uint8_t counter, uint8_t *centena, uint8_t *decena, uint8_t *unidad)
{
    uint8_t variable_division;            // DEFINO VARIABLE PARA DIVIDIR.
    *centena = counter / 100;             // '100' --> CENTENAS, LO DIVIDO DENTRO DE ESA MAGNITUD.
    variable_division = counter % 100;    // AISLO DECENAS DE LAS UNIDADES, UTILIZANDO UN 'MODULO'.
    *decena = variable_division / 10;     // DIVIDO MI VARIABLE DEFINIDA DENTRO DE 10, PARA OBTENER DECENAS.
    *unidad = variable_division % 10;     // VALOR DE VARIABLE DE DIVISION --> UNIDADES.
    return;
}

/*------------------------------------------------------------------------------
 * TABLA
 ------------------------------------------------------------------------------*/
// TRADUZCO VALORES DEL 0 AL 9, BINARIO --> DECIMAL
uint8_t tabla(uint8_t value_traduccion)    
{
    switch(value_traduccion)               // EN MI DIVISION, DEFINÍ PARA MI TABLA, UN 'value_traduccion'. DEPENDIENDO ESE VALOR LO BUSCO EN MI 'TABLA'.
    {
        case 0:
            return 0b00111111;
            break;
        case 1:
            return 0b00000110;
            break;
        case 2:
            return 0b01011011;
            break;
        case 3:
            return 0b01001111;
            break;
        case 4:
            return 0b01100110;
            break;
        case 5:
            return 0b01101101;
            break;
        case 6:
            return 0b01111101;
            break;
        case 7:
            return 0b00000111;
            break;
        case 8:
            return 0b01111111;
            break;
        case 9:
            return 0b01101111;
            break;
        default:
            return 0b00111111;  // COMO DEFAULT, VOLVEMOS A CERO.
            break;
    }
}