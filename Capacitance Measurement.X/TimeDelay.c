/*
 * File:   TimeDelay.c
 * Author: randy
 *
 * Created on October 15, 2021, 3:13 PM
 */


#include <xc.h>
#include <p24F16KA101.h>

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2

void configTimerInterrupt();
void configTimers();
void NewClk(unsigned int clkval);

void NewClk(unsigned int clkval)  
{
    char COSCNOSC;
    
    if (clkval == 8)  //8MHz
    {
        COSCNOSC = 0x00;
    }
    else if (clkval == 500) // 500 kHz
    {
        COSCNOSC = 0x66;
    }
    else if (clkval== 32) //32 kHz
    {
        COSCNOSC = 0x55; 
    }
    else // default 32 kHz
    {
        COSCNOSC = 0x55;
    }
        // Switch clock to 500 kHz
     SRbits.IPL = 7;  //Disable interrupts
     CLKDIVbits.RCDIV = 0;  // CLK division = 0
     __builtin_write_OSCCONH(COSCNOSC);   // (0x00) for 8MHz; (0x66) for 500kHz; (0x55) for 32kHz;
     __builtin_write_OSCCONL(0x01);
     OSCCONbits.OSWEN=1;
     while(OSCCONbits.OSWEN==1)
     {} 
     SRbits.IPL = 0;  //enable interrupts
}

void Delay_ms(float time_ms)
{
    NewClk(32);
    configTimerInterrupt();
    configTimers();
    TMR2 = 0; // Timer Cleared
    PR2 = 16 * time_ms;// PR2 Calculation
    T2CONbits.TON = 1; // Start Clock
    
    Idle(); //Idle until a interrupt is handled
    T2CONbits.TON = 0; //Turn off clock
    TMR2 = 0;
    NewClk(8);
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
    //T2CONbits.TCKPS = 0b00; // Sets Pre-scaler to 1:1
}

    
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    T2CONbits.TON = 0; // Stops Timer
    
}