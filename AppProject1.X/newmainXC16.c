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
#include <stdbool.h>

#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"



//different modes of code
#define sleep 0 
#define setMinutes 1
#define setSeconds 2
#define PB3 3
#define RESET 4
#define shortPress 6
#define COUNTDOWN 7
#define ALARM 8

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
void Delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
void Display();

int mode = 0; //states of the code
int prevMode = 1;

//stopwatch variables
int minutes = 0; 
int seconds = 0;
int milliseconds = 0;

//Toggling Flags
int waitingFlag = 0; //to indicate if we are waiting for the 3 second PB3 press to be completed
bool countDownToggle = false; //toggles between countdown and pause depending on PB3 short press --> 0 for 
int LEDFlag = 0;

//Display variables
char tempmins[2] = ""; //holds temporary strings for formatting
char tempsecs[2] = "";
char tempmilli[4] ="";
char display[19] = "\r";

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
    while(1){
        
        switch(mode){
            
            case setMinutes:
                //increment minute count by 1 min from 0 to 59 and display on PC
                //Disp2String("In Mode 1=Minutes");
                
                Disp2String("                       "); //clear the display
                if (minutes>=59)
                    minutes = 0;
                else
                    minutes++;
                Display();
                Delay_ms(150);
                break;
            
            case setSeconds:
                //seconds counter increments by 1 sec from 0 to 59 and display on PC
                //Disp2String("\rIn Mode 2=Seconds");
                Disp2String("                       "); //clear the display

                if (seconds>=59)
                    seconds = 0;
                else
                    seconds++;
                Display();
                Delay_ms(150);
                break;
                
            case PB3: //goes into this state when PB3 pressed to config timer. If waitingFlag is still 1 by the time the delay is over, button was pressed for 3 seconds
            
                waitingFlag = 1; 
                Delay_ms(3000); //if button is released during the time, CN interrupt is called and will toggle waiting flag
                if (waitingFlag == 1){ // if waitingFlag = 1 at this point, the CN interrupt was not called
                    waitingFlag =0;
                    mode =  RESET; //go clear the counter
                    prevMode = PB3;
                }
                
                else {
                    mode = shortPress; // when waitingFlag = 0, the CN interrupt was called meaning button was let go 
                    prevMode = PB3;
                }
                break;
                
            case shortPress: //this handles short button presses --> toggle between pause and start countdown
                countDownToggle = !countDownToggle;
                if (countDownToggle)
                    mode = COUNTDOWN;
                else
                    mode = sleep;
                prevMode = shortPress;
                break;
            
            case COUNTDOWN:
                milliseconds--;
                if(milliseconds < 0){
                    milliseconds = 999;
                    seconds--;
                    if(seconds <0 ){
                    seconds = 59;
                    minutes--;
                    }   
                    Display();
                }
                
                
               
                
                else if(seconds == 0 && minutes == 0 && milliseconds ==0){
                    mode = ALARM;
                    prevMode = COUNTDOWN;
                    break;
                }
                
                //Display();
                //LEDFlag = 1-LEDFlag; //will switch between 1 and 0 
                //LATBbits.LATB8 = LEDFlag; // Turns ON LED connected to port RB8
                Delay_ms(1);
                //LED BLINK
                break;
                
            case sleep: //idle state, where clock speed is reduced and we display minutes
                if (prevMode!=mode){
                    Display();
                    NewClk(32);
                    prevMode = 0;
                }
                
                
                
                break;
                
            case RESET:
                //sets minutes and seconds to 0 
                minutes = 0; 
                seconds = 0;
                milliseconds = 0;
                if (prevMode!=mode){
                    Display();
                prevMode = RESET;
                }
                break;

                                
            case ALARM:
                // display alarm message 
                LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8
                if (prevMode != mode){
                    Display(); 
                    Disp2String("-- ALARM");
                }
          
                break;
                
        }
        
        
    }
    
    return 0;
}


void Display(){
    //PURPOSE: To display the stopwatch minutes and seconds in the form 00m:00s
    //It does so by formatting all information into a single string
    strcpy(tempmins, ""); //clear temporary variables
    strcpy(tempsecs, "");

    sprintf(tempmins, "%02d", minutes); //convert integer to string
    strcat(display, tempmins);
    strcat(display, "m");

    strcat(display, ":");
    
    sprintf(tempsecs, "%02d", seconds);//convert integer to string
    strcat(display, tempsecs);
    strcat(display, "s   ");
    
    strcat(display, ":");

    sprintf(tempmilli, "%04d", milliseconds);//convert integer to string
    strcat(display, tempmilli);
    strcat(display, "ms   ");
    

    Disp2String(display);
    strcpy(display, "\r"); //clears the display variable by setting it only equal to "\r"
    
}


//CN Interrupt (for IO Change Notification)
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void){ 
    if(IFS1bits.CNIF == 1) //if the flag is set, means IO changed 
    {
        prevMode = mode;
        DebounceButtons();
        mode = IOCheck();
        
        if (mode == 0 && waitingFlag ==1){
           //this means button press let go before 3 seconds
            prevMode = PB3;
            mode =  shortPress;
            waitingFlag = 0;
        }
    }
    LATBbits.LATB8 = 0; // Turns off LED connected to port RB8

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

