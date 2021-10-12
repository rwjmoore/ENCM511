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
#include "ChangeClk.h"

/*
 * 
 */

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);
void delay_ms(uint16_t time);
void NewClk(unsigned int clkval); 


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
    NewClk(8);
    
    while(1)
    {
        /*if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else */if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1)
        {
            LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
            delay_ms(10000);
            LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        }
        /*else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0)
        {
            LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
        }
        else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1)
        {
            LATBbitsLATB8 = 0; // Turns OFF LED connected to port RB8
        }
        else
        {.
            LATBbits.LATB8 = 0;
        }*/
    } 
    return;
}

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

void delay_ms(uint16_t time)
{
    PR2 = 8000000.0*(time/(1000.0*2.0));
    TMR2 = 0;
    T2CONbits.TON = 1;
    check = 2;
    Idle();
    check = 3;
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    T2CONbits.TON = 0; // Stops Timer
    Timer2Flag = 1; // Global Flag
    check = 4;
}



