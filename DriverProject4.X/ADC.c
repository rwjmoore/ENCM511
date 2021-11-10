/*
 * File:   ADC.c
 * Author: Eman2
 *
 * Created on November 2, 2021, 3:55 PM
 */


#include "xc.h"
#include "ADC.h"
#include "UART2.h"

uint64_t sampleOutput = 0;
uint16_t counter = 0;
uint16_t ADCValue = 0;  

void configTimerInterrupt();
void configTimers();
void collectSamples();
void configADCInterrupt();
uint16_t getAverage();

void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void);

uint16_t do_ADC(void)
{
    uint16_t ADCValue ; // 16 bit register used to hold ADC converted digital output ADC1BUF0

    /* ------------- ADC INITIALIZATION ------------------*/

    // Configure ADC by setting bits in AD1CON1 register ? slides 13-14
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.FORM = 00; // Sets the data output format to int
    AD1CON1bits.SSRC = 0b111; // Software triggered ADC Conversion
    AD1CON1bits.ASAM = 0; // Sampling starts when SAMP Bit set
    
    // Configure ADC by setting bits in AD1CON1 as shown in slides 15-16
    AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref
    AD1CON2bits.CSCNA = 0; // Prevent input scanning
    /* * */ AD1CON2bits.SMPI = 0b0000; // Interrupts  
    
    AD1CON3bits.ADRC = 0; // Use system clock
    
    //Configure the ADC?s sample time by setting bits in AD1CON2 shown in slide 17  
    AD1CON2bits.BUFM = 0; // Buffer is configured as  16 word buffer
    AD1CON2bits.ALTS = 0; // Make sure to use MUX A input setting
    
    //Configure the ADC?s sample time by setting bits in AD1CON3
    AD1CON3bits.ADRC = 0; // System Clock - A/D Conversion Clock Source Bit
    AD1CON3bits.SAMC = 0b10000; // 31 TAD - Auto Sample Time Bits
    AD1CON3bits.ADCS = 1;  
    // Ensure sample time is 1/10th of signal being sampled
    // Select and configure ADC input as shown in slides 18-20
    
    AD1CHSbits.CH0NA = 0; // Channel 0 negative input select for MUX A
    AD1CHSbits.CH0SA = 0b0101; // Channel 0 positive input select for MUX A
    
    AD1PCFGbits.PCFG5 = 0; // Sets pin to Analog Mode
    AD1CSSL = 0;
        
/* ------------- ADC SAMPLING AND CONVERSION ------------------*/
    
    ADCValue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    
    AD1CON1bits.SAMP=0; //Stop sampling
    AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
    //Disp2String("\r\nADC Set Up Complete!");
    return (ADCValue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}

void collectSamples()
{
    configADCInterrupt();
    
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
    while(AD1CON1bits.DONE == 0)
    {
        Nop();
    }
}

void configADCInterrupt()
{
    // Timer Interrupts Setups:
    IPC3bits.AD1IP = 7; // Interrupt Priority set to 7
    
    IEC0bits.AD1IE = 1; // Enable Interrupt
    IFS0bits.AD1IF = 0; // Interrupt Flag Status Register Cleared
}

void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void)
{
    if (counter < 1000)
    {
        /*Disp2String("\r\nCounter: ");
        Disp2Dec(counter);
        Disp2String("\n\n");*/
        IFS0bits.AD1IF = 0; // Clear Flag

        sampleOutput += ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
        
        counter++;
        /*Disp2String("\rDone!");
        Disp2String("\n\n");*/
    }
    else
    {
        Disp2String("\r\nText Check 2!\n");
        
        // Return new Average:
        average = sampleOutput / 1000;
        AD1CON1bits.ADON = 0; // turn off ADC module
        AD1CON1bits.SAMP=0; // Stop Sampling
        counter = 0; // Reset counter
        
        Disp2String("\r\nADC Average Output: ");
        Disp2Dec(getAverage());
    }
}

uint16_t getAverage()
{
    return average;
}

void reset()
{
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.SAMP=1; // Start Sampling
    sampleOutput = 0;
    counter = 0;
    average = 0;
}

