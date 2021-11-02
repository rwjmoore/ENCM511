/*
 * File:   IOs.c
 * Author: randy
 *
 * Created on October 15, 2021, 3:06 PM
 */
#include <xc.h>
#include <p24F16KA101.h>
#include "TimeDelay.h"
#include "UART2.h"


void IOCheck()
{
    if (PORTAbits.RA3 != 0)
    {
        uint16_t ADCValue = do_ADC();
        Disp2String("\r.|.ADC.Value.=.");
        Disp2Dec(ADCValue);
        Disp2String(".|.");
    }
    else if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
    {
        Disp2String("\rPB1.is.pressed");
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(500);     // Delay for 0.5 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(500);     // Delay for 0.5 second
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
    {
        Disp2String("\rPB2.is.pressed");
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(2000);     // Delay for 2 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(2000);     // Delay for 2 second
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
    {
        Disp2String("\rPB3.is.pressed");
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
        Delay_ms(3000);     // Delay for 3 second
        LATBbits.LATB8 = 0; // Turns OFF LED connected to port RB8
        Delay_ms(3000);     // Delay for 3 second
    }
    else if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) // 3 buttons pressed
    {
        Disp2String("\rAll.PBs.pressed");
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
    }
    else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // No buttons pressed
    {        
        Disp2String("\rNothing.pressed");
        LATBbits.LATB8 = 0; // Turns ON LED connected to port RB8
    }
    else // 2 Buttons pressed
    {
        if (PORTBbits.RB4 == 1)
        {
            Disp2String("\rPB1.and.PB2.are.pressed");;     
        }
        else if(PORTAbits.RA4 == 1)
        {
            Disp2String("\rPB1.and.PB3.are.pressed");     
        }
        else if(PORTAbits.RA2 == 1)
        {
            Disp2String("\rPB2.and.PB3.are.pressed");     
        }
        LATBbits.LATB8 = 1; // Turns OFF LED connected to port RB8
    }
}

void IOinit()
{
    // Outputs:
    TRISBbits.TRISB8 = 0; // Makes GPIO RB8 a digital output
    
    // Inputs:
    TRISAbits.TRISA2 = 1; // Makes GPIO RA2 a digital input
    TRISAbits.TRISA3 = 1; // Makes POT Bit RA3 a digital input
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

uint16_t do_ADC(void)
{
    uint16_t ADCvalue ; // 16 bit register used to hold ADC converted digital output ADC1BUF0

    /* ------------- ADC INITIALIZATION ------------------*/

    // Configure ADC by setting bits in AD1CON1 register ? slides 13-14
    AD1CON1bits.ADON = 1; // turn on ADC module
    AD1CON1bits.FORM = 00; // Sets the data output format to int
    AD1CON1bits.SSRC = 111; // Software triggered ADC Conversion
    AD1CON1bits.ASAM = 0; // Sampling starts when SAMP Bit set
    
    // Configure ADC by setting bits in AD1CON1 as shown in slides 15-16
    AD1CON2bits.VCFG = 0b000; // Selects AVDD, AVSS (supply voltage to PIC) as Vref
    AD1CON2bits.CSCNA = 0; // Prevent input scanning
    /* * */ AD1CON2bits.SMPI = 0000; // Interrupts  
    
    AD1CON3bits.ADRC = 0; // Use system clock
    
    //Configure the ADC?s sample time by setting bits in AD1CON2 shown in slide 17  
    AD1CON2bits.BUFM = 0; // Buffer is configured as  16 word buffer
    AD1CON2bits.ALTS = 0; // Make sure to use MUX A input setting
    
    //Configure the ADC?s sample time by setting bits in AD1CON3
    /* * */ AD1CON3bits.ADRC = 0; // System Clock - A/D Conversion Clock Source Bit
    AD1CON3bits.SAMC = 11111; // 31 TAD - Auto Sample Time Bits
    //AD1CON3bits.ADCS = 0;  
    // ADD CODE // Ensure sample time is 1/10th of signal being sampled
    // ADD CODE // Select and configure ADC input as shown in slides 18-20
    
    AD1CHSbits.CH0NA = 0; // Channel 0 negative input select for MUX A
    AD1CHSbits.CH0SA = 0101; // Channel 0 positive input select for MUX A
    
    AD1PCFGbits.PCFG5 = 0; // Sets pin to Analog Mode
    AD1CSSL = 0;
    
    
        
/* ------------- ADC SAMPLING AND CONVERSION ------------------*/

    AD1CON1bits.SAMP=1; //Start Sampling, Conversion starts automatically after SSRC and SAMC settings
    while(AD1CON1bits.DONE==0)
    {}
    
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point
    AD1CON1bits.SAMP=0; //Stop sampling
    AD1CON1bits.ADON=0; //Turn off ADC, ADC value stored in ADC1BUF0;
    return (ADCvalue); //returns 10 bit ADC output stored in ADC1BIF0 to calling function
}
