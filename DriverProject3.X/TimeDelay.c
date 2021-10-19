/*
 * File:   TimeDelay.c
 * Author: randy
 *
 * Created on October 15, 2021, 3:13 PM
 */


#include <xc.h>
#include <p24F16KA101.h>


void configTimerInterrupt();
void configTimers();

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