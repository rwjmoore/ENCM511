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

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);
void delay_ms(uint16_t time);

int Timer2Flag = 0;
int check = 0;

void main(void)
{
    check = 1;
    
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
    
    // Timer Interrupts Setups:
    IPC1bits.T2IP2 = 1;
    IPC1bits.T2IP1 = 1;
    IPC1bits.T2IP0 = 1;
    
    IEC0bits.T2IE = 1;
    
    IFS0bits.T2IF = 0;
    
    // Clock Setting:
    OSCCONbits.COSC0 = 0;
    OSCCONbits.COSC1 = 0;
    OSCCONbits.COSC2 = 0;
    
    OSCCONbits.OSWEN = 0;
    
    // Timer Set Up:
    T2CONbits.T32 = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TSIDL = 0;
    T2CONbits.TCKPS0 = 0;
    T2CONbits.TCKPS1 = 0;
    
    PR2 = 0;
    TMR2 = 0;
    
    while(1)
    {
        if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
            delay_ms(10000);
            //LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        }
        else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        }
        else
        {
            LATBbits.LATB8 = 0;
        }
    } 
    return;
}

void delay_ms(uint16_t time)
{
    TMR2 = 0;
    PR2 = 8000000*(time/(1000*2));
    T2CONbits.TON = 1;
    check = 2;
    Idle();
    //_T2Interrupt();
    check = 3;
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    T2CONbits.TON = 0; // Stops Timer
    Timer2Flag = 1; // Global Flag
    check = 4;
}



