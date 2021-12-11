/*
 * File:   newmainXC16.c
 * Author: randy
 *
 * Created on November 28, 2021, 1:00 PM
 * APP PROJECT 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include <string.h>

#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "ADC.h"
#include "Comparator.h"

//MODES OF OPERATION
#define frequency 0
#define Voltmeter 1
#define Ohmeter 2
#define capacitanceMeter 3

//Function Prototypes 
uint32_t collectSamples(int input);
uint32_t do_ADC(int input);
void Delay_ms(uint32_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
void startCapCharge();
void discharge();
int mode = 0; //state variable

int main(void) {
    
    IOinit();
    NewClk(8); //set clock to 8Mhz
    // ----- Configuring the CN (IO Change) Interrupt ---- //
    IPC4bits.CNIP0 = 0; //Configure CN interrupt to priority 6 (110)
    IPC4bits.CNIP1 = 1;
    IPC4bits.CNIP2 = 1;
    IFS1bits.CNIF = 0; //clear flag bit for CN interrupt
    IEC1bits.CNIE = 1; //Input change notification interrupt enable bit 
    // ------       Config IO interrupt DONE       ------ //
    
    float ADCVoltage = 0;
    float ADCResistance = 0;
    float resistorCurrent = 0;
    
    char display[50] = "\r";
    
    while(1)        
    {
        ADCVoltage = 0;
        ADCResistance = 0;
        
        switch(mode){
            
            case frequency:
                NewClk(32);
                
                comparator_Init(1);
                
                memset(&display, '\0', sizeof(display));
                sprintf(display, "\rFrequency:.%5.3f.Hz.........................", compare());
                Disp2String(display);
                                
                break;
            
            case Voltmeter:
                
                NewClk(32);
                
                do_ADC(5);
                ADCVoltage = collectSamples(5);
                
                memset(&display, '\0', sizeof(display));
                sprintf(display, "\rADC.Average.Voltage:.%5.3f..V.......", ADCVoltage/1000);
                Disp2String(display);
                
                Delay_ms(1000);
                
                break;
                
            case Ohmeter:
                
                NewClk(32);
                
                do_ADC(11);
                ADCVoltage = collectSamples(11);
                
                resistorCurrent = (3.25 - ((float)ADCVoltage)/1000)/1000;
                
                ADCResistance = (3.25 - (resistorCurrent*1000))/(resistorCurrent);
                          
                memset(&display, '\0', sizeof(display));
                sprintf(display, "\rADC.Average.Resistance:.%5.3f..Ohm.......", ADCResistance);
                Disp2String(display);
                
                Delay_ms(1000);
                
                break;
                
           case capacitanceMeter:
                memset(&display, '\0', sizeof(display));
                //sprintf(display, "\rCapacitance:.Charging.................");
                Disp2String("\rCapacitance:............uF");
                discharge();
                startCapCharge();
                Delay_ms(6000);
                
                float capacitance = getInterruptedTime()  ;
              
                memset(&display, '\0', sizeof(display));
                sprintf(display, "\rCapacitance:.%f.uF.........................", capacitance / 2 );
                Disp2String(display); 
                discharge();
                break;           
                
                
            
        }
                
         
        
    }
    
    
    return 0;
}

//CN Interrupt (for IO Change Notification)
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void){ 
    if(IFS1bits.CNIF == 1) //if the flag is set, means IO changed 
    {
        //prevMode = mode;
        DebounceButtons();
        mode = IOCheck();
    }
    IFS1bits.CNIF = 0; //setting the interrupt flag back to 0 
    Nop(); //Apparently does nothing, but reserves space for delay or execution of other code
    
} 

void DebounceButtons()
{
    if (PORTAbits.RA2 == 0 || PORTAbits.RA4 == 0 || PORTBbits.RB4 == 0)
    {
        int resCheck = 0;
        int pastResult = 0;
        int result = 0;
        while(1)
        {
            if(PORTAbits.RA2 == 0 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // PB1 (RA2) is pressed
            {
                result = 1;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 1) // PB2 (RA4) is pressed
            {
                result = 2;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 0) // PB3 (RB4) is pressed
            {
                result = 3;
            }
            else if(PORTAbits.RA2 == 1 && PORTAbits.RA4 == 1 && PORTBbits.RB4 == 1) // No buttons pressed
            {
                result = 0;
            }
            else if (PORTAbits.RA2 == 0 && PORTAbits.RA4 == 0 && PORTBbits.RB4 == 0) // 3 buttons pressed
            {
                result = 5;
            }
            else // 2 Buttons 
            {
                result = 4;
            }

            if (pastResult == result)
            {
                resCheck++;
            }
            if(resCheck > 2)
            {
                break;
            }

            pastResult = result;
        }
    }
    
    
}

void startCapCharge(){
    //calculating capacitance
    //output voltage pin of 3.25V is RA6
    TRISBbits.TRISB9 = 0; //Make RB9 digital output // this is what supplies voltage
    //TRISBbits.TRISB2 = 0; //Make RA6 digital output // this is what discharges the capacitor
    Delay_ms(100);
    //configure the comparator to interrupt at 0.63*Vdd = 2.0475V on same pin as frequency
    comparator_Init(0); //false sets it to capacitance mode
    
    //start charging the capacitor

    LATBbits.LATB9 = 1; // turn on voltage to 3.3V
    CM1CONbits.CON = 1; 	// Turn Comparator ON
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
}