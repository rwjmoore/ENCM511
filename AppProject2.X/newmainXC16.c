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



//MODES OF OPERATION
#define sleep 0
#define Voltmeter 1
#define Ohmeter 2
#define capacitanceMeter 3
#define stopWatch 4

//Function Prototypes 
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
void Delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
void Display();


//VARIABLES
int mode = 0; //state variable

//timer related variables
int minutes = 0;
int seconds = 0; 
int milliseconds = 0; 





int main(void) {
    
    while(1){
        
        switch(mode){
            
            case sleep:
                NewClk(32);                
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
