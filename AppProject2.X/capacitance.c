/*
 * File:   capacitance.c
 * Author: randy
 *
 * Created on December 7, 2021, 4:44 PM
 */


#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include <string.h>
#include <stdbool.h>

#include "UART2.h"
#include "Comparator.h"
#include "TimeDelay.h"
#include "capacitance.h"
//Discharges the charged capacitance after the calculation is completed 

void comparator_Init(bool mode);

void startCapCharge(){
    //calculating capacitance
    //output voltage pin of 3.25V is RA6
    TRISBbits.TRISB9 = 0; //Make RB9 digital output // this is what supplies voltage
    //TRISBbits.TRISB2 = 0; //Make RA6 digital output // this is what discharges the capacitor
    Delay_ms(100);
    //configure the comparator to interrupt at 0.63*Vdd = 2.0475V on same pin as frequency
    comparator_Init(false); //false sets it to capacitance mode
    
    //start charging the capacitor

    LATBbits.LATB9 = 1; // turn on voltage to 3.3V
    CM1CONbits.CON = 1; 	// Turn Comparator ON

    //start timer, causes code to go into idle
}

void discharge(){
    //after displaying calculation in main, we need to discharge the capacitor 
    TRISBbits.TRISB9 = 0; //Make RA6 digital output // this is what supplies voltage
    LATBbits.LATB9 = 0; // turn on voltage to 3.3V
    Delay_ms(1000);
    Delay_ms(1000);
    Delay_ms(1000);
    
    Delay_ms(1000);
    Delay_ms(1000);
    Delay_ms(1000);
    
    //shut off comparator module 
    //CM1CONbits.CON = 1; 	// Turn Comparator OFF
}

