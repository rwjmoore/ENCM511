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


/*
 PURPOSE: To initialize the comparator on the correct pin
 
 */
/*int comparatorInit(){
    //let's use RB1 as the comparator input 
    
    TRISAbits.TRISA6 = 0; 	// for C2Out
    
    TRISBbits.TRISB1 = 1; 	//Make RB1 an input
    AD1PCFGbits.PCFG3 = 0; 	//make AN3 an analog input
    IEC1bits.CMIE = 0;      	//disable interrupts while setting up pin
    CM2CONbits.COE = 1;  	//enable output pin
    CM2CONbits.CPOL = 0; 	//invert polarity
    CM2CONbits.EVPOL = 0;
    CM2CONbits.CREF = 0; 	//C2InA pin is +in
    CM2CONbits.CCH = 3; 	//-in pin is connected to Vbg/2
    IFS1bits.CMIF = 0; 		//clear IF flag
    CM2CONbits.CON = 1; 	//turn comparator on
    
    
    TRISAbits.TRISA2 = 1; 	// A2 set as input on port pin
    AD1PCFGbits.PCFG4 = 0; 	// Set input to Analog
    IEC1bits.CMIE = 0; 		// IE Off so no interrupt occurs from setup
    CM1CONbits.COE = 0; 	// Disable output pin
    CM1CONbits.CPOL = 0; 	// Standard sense. +In High ==> Out High
    CM1CONbits.EVPOL = 2; 	// Event detected on output edge falling
    CM1CONbits.CREF = 1; 	// +IN is internal CVRef
    CM1CONbits.CCH = 0; 	// -IN is C1INB Pin
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    
    CVRCON = 0x88; 		// CVRef = (1/2) * (AVdd - AVss)
    
    CM1CONbits.CEVT = 0;
    IFS1bits.CMIF = 0; 		// Clear IF after set-up

    // When C1INB RISES above AVdd/2,
    // C1OUT falls & edge is counted.
       
    IEC1bits.CMIE = 1; 

    
    return 0;
}*/

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

