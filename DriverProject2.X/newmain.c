/* 
 * File:   newmain.c
 * Author: Eman2
 *
 * Created on October 5, 2021, 3:11 PM
 */

#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include "xc.h"

/*
 * 
 */

/* How to use debug (view variables) 
 Specifically PR2 calculation 
 * time gate accumulation
 */

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);
void Delay_ms(uint16_t time);
void NewClk(unsigned int clkval); 
void IOinit();
void IOCheck();

void configTimers();
void configTimerInterrupt();


int Timer2Flag = 0; // Global Timer Flag Variable
int check = 0;

void main(void) // *ASK!!!: How to read and debug variables*
{
    IOinit();
    
    while(1)
    {
        // Button Debouncing:
        
            
        IOCheck();
    } 
    return;
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

void DebounceButtons()
{
    if (PORTAbits.RA2 == 0 || PORTAbits.RA4 == 0 || PORTBbits.RB4 == 0)
    {
        int resCheck = 0;
        int pastResult = 0;
        int result = 0;
        while(1)
        {
            if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
            {
                result = 1;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
            {
                result = 2;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
            {
                result = 3;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // No buttons pressed
            {
                result = 0;
            }
            else // Multiple buttons pressed
            {
                result = 4;
            }

            if (pastResult == result)
            {
                resCheck++;
            }
            if(resCheck > 3)
            {
                break;
            }

            pastResult = result;
            Delay_ms(10);
        }
    }
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    T2CONbits.TON = 0; // Stops Timer
    Timer2Flag = 1; // Global Flag
    check = 4;
}

void Delay_ms(uint16_t time_ms)
{
    configTimerInterrupt();
    configTimers();
    
    T2CONbits.TON = 1; // Start Clock
    TMR2 = 0; // Timer Cleared
    PR2 = 8 * time_ms;// PR2 Calculation
    Idle(); //Idle until a interrupt is handled
    T2CONbits.TON = 0; //Turn off clock
    TMR2 = 0;
    check = 2;
    check = 3;
}

void configTimerInterrupt()
{
    // Timer Interrupts Setups:
    IPC1bits.T2IP = 7; // Interrupt Priority set to 7
    
    IEC0bits.T2IE = 1; // Enable Interrupt - Register 0
    IFS0bits.T2IF = 0; // Interrupt Flag Status Register Cleared
}

void configTimers()
{
    // Timer Set Up:
    T2CONbits.TSIDL = 0; // Continue module operation when Idle
    T2CONbits.T32 = 0; // Timer2 acts as a 16-bit timer
    T2CONbits.TCS = 0; // Use Internal Clock
    T2CONbits.TCKPS = 3; // Sets Pre-scaler to 11
}