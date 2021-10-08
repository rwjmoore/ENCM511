/* 
 * File:   newmain.c
 * Author: Eman2
 *
 * Created on October 5, 2021, 3:11 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>

/*
 * 
 */

//void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void);

void main(void)
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
    //OSCCONbits.OSWEN = 0; // Turn off clock
    
    LATBbits.LATB8 = 0;
    
    while(1)
    {
        if(PORTAbits.RA2 == 1)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else
        {
            LATBbits.LATB8 = 0;
        }
        /*else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else
        {
            LATBbits.LATB8 = 0;
        }*/
        /*else
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }*/
    }
        
    
    return;
}

/*void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
    if(IFS1bits.CNIF == 1)
    {
        printf("Interrupt!");
        if (PORTAbits.RA2 == 0)// && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1;
        }
        /*else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1;
        }
        else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)
        {
            LATBbits.LATB8 = 1;
        }
        
    }
    IFS1bits.CNIF = 0; //clear IF flag
    Nop();
}*/



