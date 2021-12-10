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




int IOCheck()
{//Checks which pushbuttons are pressed and returns the value of the one that is pressed
    
//returns the following based on this lookup table
    /* 
     None Pressed (Or button released) = 0 = sleep
     PB1 Pressed = 1 = Voltmeter
     PB2 Pressed = 2 = Ohmeter 
     PB3 Pressed = 3 = capacitanceMeter 
     Any combination = 0 
     
     */
    if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
    {
        //Disp2String("\rPB1_is_pressed");
        LATBbits.LATB8 = 1;
        return 1;
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
    {
        //Disp2String("\rPB3_is_pressed");
        LATBbits.LATB8 = 1;
        return 3;
    }
    else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
    {
        //Disp2String("\rPB2_is_pressed");
        LATBbits.LATB8 = 1;
        return 2;
    }
    else if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) // 3 buttons pressed
    {
        Disp2String("\rAll_PBs_pressed");
        LATBbits.LATB8 = 1;
        return 0;
    }
    else if (PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // No buttons pressed
    {        
        //Disp2String("\rNothing_pressed"); 
        LATBbits.LATB8 = 0;
     
        
        
        return 0; //let's us check that the button has been released
    }
    else // 2 Buttons pressed
    {
        LATBbits.LATB8 = 1;
        if (PORTBbits.RB4 == 1)
        {
            //Disp2String("\rPB1_and_PB2_are_pressed"); 
            return -1;
        }
        else if(PORTAbits.RA4 == 1)
        {
            //Disp2String("\rPB1_and_PB3_are_pressed");
            return -1;
        }
        else if(PORTAbits.RA2 == 1)
        {
            //Disp2String("\rPB2_and_PB3_are_pressed");
            return -1;
        }
    }
    return 0;
    
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

