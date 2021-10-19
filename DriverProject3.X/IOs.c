/*
 * File:   IOs.c
 * Author: randy
 *
 * Created on October 15, 2021, 3:06 PM
 */
#include <xc.h>
#include <p24F16KA101.h>
#include "TimeDelay.h"


void IOCheck()
{
    if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(1000);     // Delay for 1 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(1000);     // Delay for 1 second
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(2000);     // Delay for 2 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(2000);     // Delay for 2 second
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(3000);     // Delay for 3 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(3000);     // Delay for 3 second
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // No buttons pressed
    {
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
    }
    else // Multiple buttons pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
    }
}

void IOinit()
{
    // Outputs:
    TRISBbits.TRISB8 = 0; // Makes GPIO RB8 a digital output
    
    // Inputs:
    TRISAbits.TRISA2 = 1; // Makes GPIO RA2 a digital input
    TRISAbits.TRISA4 = 1; // Makes GPIO RA4 a digital input
    TRISBbits.TRISB4 = 1; // Makes GPIO RB4 a digital input
            
    // Pull Up Enable Set Up:
    CNEN1bits.CN1IE = 1; // Enables pull up resistor on RB4 
    CNPU1bits.CN1PUE = 1; // Enables pull up resistor on RA4 
    
    CNEN1bits.CN0IE = 1; // Enables pull up resistor on RB4
    CNPU1bits.CN0PUE = 1; // Enables pull up resistor on RA4
    
    CNEN2bits.CN30IE = 1; // Enables pull up resistor on RA2
    CNPU2bits.CN30PUE = 1; // Enables pull up resistor on RA2
    AD1PCFG = 0xFFFF; // Turn off clock on pin RA2
    
    LATBbits.LATB8 = 0;
}

