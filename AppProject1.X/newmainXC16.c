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
#include "xc.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
void Delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)



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
        
        
        
    }
    
    return 0;
}

//CN Interrupt (for IO Change Notification)
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void){ 
    
    if(IFS1bits.CNIF == 1) //if the flag is set, means IO changed 
    {
        
        Disp2String("\r\n\nButtonPressed \n");

        //Code to determine which of the pins was pushed 
    }
    IFS1bits.CNIF = 0; //setting the interrupt flag back to 0 
    Nop(); //Apparently does nothing, but reserves space for delay or execution of other code
    
} 
