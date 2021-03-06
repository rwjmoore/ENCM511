/* 
 * File:   newmain.c
 * Author: Eman2
 *
 * Created on October 5, 2021, 3:11 PM
 */
//
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p24F16KA101.h>
#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "ADC.h"
#include "UART2.h"

/*
 * 
 */
//
/* How to use debug (view variables) 
 Specifically PR2 calculation 
 * time gate accumulation
 */

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void);
void Delay_ms(uint16_t time_ms);
void DebounceButtons();
void ADC_Delay(uint16_t);
void DiplayBar(uint64_t val);
uint16_t do_ADC();
uint64_t collectSamples();

int ADC10SecFlag = 1;
int Timer2Flag = 0; // Global Timer Flag Variable

int main(void) // *ASK!!!: How to read and debug variables*
{
    IOinit();
    NewClk(32);
        
    while(1)
    {
        if(ADC10SecFlag)
        {
            // Button Debouncing:
            DebounceButtons();
            ADC10SecFlag = IOCheck();
        }
        else
        {
            Disp2String("\n\n");
            
            int i;
            for (i = 0; i < 10; i++)
            {
                do_ADC();
                uint64_t average = collectSamples();
                int aveInt = average / 1000;
                int aveDec = average % 1000;
                
                Disp2String("\rADC Average Voltage:.");
                
                Disp2Dec(aveInt);
                Disp2String(".V...");
                Disp2Dec(aveDec);
                Disp2String(".mV");
                
                Disp2String("...");
                //DiplayBar(average);
                Delay_ms(1000);
            }
            ADC10SecFlag = 1;
        }       
        
        /*Disp2String("\r\n\nFlag: ");
        Disp2Dec(ADC10SecFlag);*/
    }
    return 0;
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
            if(resCheck > 10)
            {
                break;
            }

            pastResult = result;
            Delay_ms(10);
        }
    }
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0; // Clear Timer 2 Flag
    T2CONbits.TON = 0; // Stops Timer
    Timer2Flag = 1; // Global Flag
    ADC10SecFlag = 0;
    TMR2 = 0;
}

// 3.25 / 65535

void DiplayBar(uint64_t val)
{
    int i = 0;
    // Bar Elements:
    
    for (i = 700; i < val; i+=10)
    {
        Disp2String("[*]");
    }
    // White Space:
    for (i = i; i < 840; i+=10)
    {
        Disp2String("...");
    }
}