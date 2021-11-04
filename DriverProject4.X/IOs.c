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
#include "ADC.h"

uint16_t counter = 0;
double average = 0;
uint16_t ADCValue = 0;

void IOCheck()
{
    int total_samples = 1000;
    if (counter < total_samples)
    {   
        ADCValue = do_ADC();
        Delay_ms(1);
       
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
        double val = average / total_samples;
        /*while (val)
        {
            Disp2String("o");
        }*/
        counter = 0;
        average = 0;
        
        // Range of ADC Values: Based on testing, the ADC values goes from 0-1015
        //Max length of barchart is 
    }
    
    
    
    
    
    
    if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
    {
        LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
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
