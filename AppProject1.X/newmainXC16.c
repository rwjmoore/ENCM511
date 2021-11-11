/*
 * File:   newmainXC16.c
 * Author: randy
 *
 * Created on November 10, 2021, 1:46 PM
 */


#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include <string.h>

#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"



//different modes of code
#define sleep 0 
#define setMinutes 1
#define setSeconds 2
#define COUNTDOWN 3

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
void Delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
void Display();

int mode = 0; //start in sleep mode
int minutes = 0;
int seconds = 0;

char tempmins[2] = ""; //holds temporary strings for formatting
char tempsecs[2] = "";

char display[19] = "\r";

int main(void) {
    IOinit();
    NewClk(8); //set clock to 8Mhz
    // ----- Configuring the CN (IO Change) Interrupt ---- //
    IPC4bits.CNIP0 = 0; //Configure CN interrupt to priority 6 (110)
    IPC4bits.CNIP1 = 1;
    IPC4bits.CNIP2 = 1;
    IFS1bits.CNIF = 0; //clear flag bit for CN interrupt
    IEC1bits.CNIE = 1; //Input change notificiation interrupt enable bit 
    // ------       Config IO interrupt DONE       ------ //
    while(1){
        
        switch(mode){
            
            case setMinutes:
                //increment minute count by 1 min from 0 to 59 and display on PC
                //Disp2String("In Mode 1=Minutes");
                if (minutes>=59)
                    minutes = 0;
                else
                    minutes++;
                Display();
                Delay_ms(100);
                break;
            
            case setSeconds:
                //seconds counter increments by 1 sec from 0 to 59 and display on PC
                //Disp2String("\rIn Mode 2=Seconds");
                if (seconds>=59)
                    seconds = 0;
                else
                    seconds++;
                Display();
                Delay_ms(100);
                break;
                
            case COUNTDOWN:
                //short press on PB3 starts or pauses the countdown 
                //long press stops the countdown and resets the timer to 0 
                //LED blinks at 1 sec interval 
                //when countdown is complete LED stays ON and displays specific message
                break;
                
            case sleep:
                //Disp2String("\r               ");
                //Display();
                
                //idle state, where clock speed is reduced and we display minutes
                break;
        }
        
        
    }
    
    return 0;
}


void Display(){
    //PURPOSE: To display the stopwatch minutes and seconds in the form 00m:00s
    strcpy(tempmins, ""); //clear temporary variables
    strcpy(tempsecs, "");


    //int sprintf(char *strValue, const char *format, [arg1, arg2, ... ]);
    sprintf(tempmins, "%d", minutes);
    strcat(display, tempmins);
    strcat(display, "m");

    strcat(display, ":");
    sprintf(tempsecs, "%d", seconds);
    strcat(display, tempsecs);
    strcat(display, "s");

    Disp2String(display);
    strcpy(display, "\r");
    
}


//CN Interrupt (for IO Change Notification)
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void){ 
    if(IFS1bits.CNIF == 1) //if the flag is set, means IO changed 
    {
        
        DebounceButtons();
        

        mode = IOCheck();
        //Disp2String("IO CheckDone");
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
            if(resCheck > 12)
            {
                break;
            }

            pastResult = result;
            //Delay_ms(1);
        }
    }
}

