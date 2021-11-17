/*
 * File:   ADC.c
 * Author: Eman2
 *
 * Created on November 2, 2021, 3:55 PM
 */


#include "xc.h"
#include "ADC.h"
#include "UART2.h"

uint16_t counter = 0;
double average = 0;
uint16_t ADCValue = 0;   

void configTimerInterrupt();
void configTimers();
uint64_t collectSamples();

void ADC_Delay(uint16_t time_ms)
{
    configTimerInterrupt();
    configTimers();
    
    T2CONbits.TON = 1; // Start Clock
    TMR2 = 0; // Timer Cleared
    PR2 = 16 * time_ms;// PR2 Calculation   
}

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
    /* * */ AD1CON3bits.ADRC = 0; // System Clock - A/D Conversion Clock Source Bit
    AD1CON3bits.SAMC = 0b10000; // 31 TAD - Auto Sample Time Bits
    AD1CON3bits.ADCS = 1;  
    // ADD CODE // Ensure sample time is 1/10th of signal being sampled
    // ADD CODE // Select and configure ADC input as shown in slides 18-20
    
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

uint64_t collectSamples()
{
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.ASAM = 0;
    uint64_t sampleOutput = 0;
    AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
    while(AD1CON1bits.DONE==0)
    {
        sampleOutput += ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    } 
    AD1CON1bits.SAMP=0;
    
    uint64_t average = sampleOutput / 1000;
    
    AD1CON1bits.ADON = 0; // turn on ADC module
    
    return average;
    
    /*int total_samples = 1000;
    if (counter < total_samples)
    {   
        ADCValue = do_ADC();
        //Delay_ms(1);

        average += ADCValue  * 3.25 / 1024;
        counter++;
    }
    else
    {
        Disp2String("\n");
        Disp2String("\r\n...ADC.Value=");
        Disp2Dec(ADCValue);
        Disp2String("\r\n...Average=");
        Disp2Dec(average);
        double val = average // total_samples;
        while (val)
        {
            Disp2String("o");
        }
        counter = 0;
        average = 0;

        // Range of ADC Values: Based on testing, the ADC values goes from 0-1015
        //Max length of barchart is 
    }*/
}
