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
#include <stdbool.h>

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
#define stopWatch 4

//Function Prototypes 
uint64_t collectSamples(int input);
uint64_t do_ADC(int input);
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
void Delay_ms(uint32_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
void Display();


//VARIABLES
int mode = 0; //state variable

//timer related variables
int minutes = 0;
int seconds = 0; 
int milliseconds = 0; 

uint64_t compare();
void comparatorInit();



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
    
    uint64_t ADCVoltage = 0;
    double ADCResistance = 0;
    double resistorCurrent = 0;
    int aveInt = 0;
    int aveDec = 0;
    
    while(1)    
    {
        ADCVoltage = 0;
        ADCResistance = 0;
        
        //Disp2String("\rFlag:.");
        //Disp2Dec(mode);
        //Disp2String("...................................");
        
        switch(mode){
            
            case frequency:
                NewClk(32);
                
                comparatorInit();
                Disp2String("\rFrequency: ");
                Disp2Dec(compare());
                Disp2String(".Hz...............................");
                
                break; 
            
            case Voltmeter:
                
                NewClk(32);
                
                Disp2String("\rADC.Average.Voltage:.");
                
                do_ADC(5);
                ADCVoltage = collectSamples(5);
                aveInt = ADCVoltage / 1000;
                aveDec = ADCVoltage % 1000;
                
                Disp2Dec(aveInt);
                Disp2String(".V...");
                Disp2Dec(aveDec);
                Disp2String(".mV");
                
                Disp2String("...");
                
                Delay_ms(1000);
                
                break;
                
            case Ohmeter:
                
                NewClk(32);
                
                do_ADC(11);
                ADCVoltage = collectSamples(11);
                
                resistorCurrent = (3.25 - ((double)ADCVoltage)/1000)/1000;
                
                ADCResistance = (3.25 - (resistorCurrent*1000))/(resistorCurrent);
                
                char display[100] = "\r";
                
                sprintf(display, "\rADC.Average.Resistance:.%5.3f..Ohm.......", ADCResistance);
                Disp2String(display);
                
                Delay_ms(1000);
                
                break;
                
            case capacitanceMeter:
                
                break;
                
            case stopWatch:
                milliseconds--;
                if(milliseconds < 0){
                    milliseconds = 999;
                    seconds--;
                    if(seconds <0 ){
                    seconds = 59;
                    minutes--;
                    }   
                }
                Delay_ms(1); //1 ms delay for the stopwatch
                
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

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    
    T2CONbits.TON = 0; // Stops Timer
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
