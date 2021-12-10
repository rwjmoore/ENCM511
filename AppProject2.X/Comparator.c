#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include <string.h>
#include <stdbool.h>

#include "UART2.h"
#include "Comparator.h"
#include "TimeDelay.h"

int eventCount = 0;

uint64_t getInterruptedTime();
void Delay_ms(uint32_t time_ms);

void comparatorInit()
{
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
    
    
    TRISAbits.TRISA4 = 1; 	// A2 set as input on port pin
    AD1PCFGbits.PCFG3 = 0; 	// Set input to Analog
    
    IEC1bits.CMIE = 0; 		// IE Off so no interrupt occurs from setup
    CM1CONbits.COE = 0; 	// Disable output pin
    CM1CONbits.CPOL = 0; 	// Standard sense. +In High ==> Out High
    CM1CONbits.EVPOL = 2; 	// Event detected on output edge falling
    CM1CONbits.CREF = 1; 	// +IN is internal CVRef
    CM1CONbits.CCH = 1; 	// -IN is C1INC Pin
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    
    CVRCON = 0x88; 		// CVRef = (1/2) * (AVdd - AVss)
    
    CM1CONbits.CEVT = 0;
    IFS1bits.CMIF = 0; 		// Clear IF after set-up

    // When C1INB RISES above AVdd/2,
    // C1OUT falls & edge is counted.
       
    IEC1bits.CMIE = 1; 
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void)
{
	IFS1bits.CMIF = 0;		// clear IF flag
    eventCount++; 
    Nop();
        
    return;
}

double compare()
{
    double eventTotal = 0;
   /* eventCount = 0;
    
    Delay_ms(1000);
    if(getInterruptedTime() < 1000)
    {
        eventTotal += eventCount;
        eventCount = 0;
    }
    
    //return 1/((double)getInterruptedTime()/1000);*/
 
    return eventTotal;
}