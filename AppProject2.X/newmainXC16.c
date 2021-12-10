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
uint64_t collectSamples(int input);
uint64_t do_ADC(int input);
void Delay_ms(uint32_t time_ms);
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)
void DebounceButtons();
uint64_t getInterruptedTime();
void Display();

double compare();
void startCapCharge();
void discharge();
void comparator_Init(int val);

//VARIABLES
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
                
                comparator_Init(1);
                
                char freqDisplay[50] = "\r";
                sprintf(freqDisplay, "\rFrequency:.%5.3f.Hz.........................", compare());
                Disp2String(freqDisplay);
                                
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
                
                char resistDisplay[50] = "\r";
                
                sprintf(resistDisplay, "\rADC.Average.Resistance:.%5.3f..Ohm.......", ADCResistance);
                Disp2String(resistDisplay);
                
                Delay_ms(1000);
                
                break;
                
            case capacitanceMeter:
        //        TRISAbits.TRISA6 = 0; //Make RA6 digital output // this is what supplies voltage
        //        LATAbits.LATA6 = 1; // turn on voltage to 3.3V

                Disp2String("\nDischarging\n");
                discharge();
                Disp2String("Starting Charging\n");
                //LATBbits.LATB8 = 1; // Turns ON LED connected to port RB8

                startCapCharge();
                Delay_ms(4000);
                
                int time = getInterruptedTime();
                //double capacitance = time / 2100;
                char capDisplay[50] = "\r";
                sprintf(capDisplay, "\nCapacitance:.%d.F.........................\n", time);
                Disp2String(capDisplay);        
                LATBbits.LATB8 = 0; // Turns ON LED connected to port RB8

                //while(eventCount==0); //verify this works!! ..dont think i need this bc the timer should snap me out
                //Disp2String("Starting Discharging");
                discharge();


                //display our capacitance reading (done in interrupt)

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

/*int eventCount = 0;
double currentFrequency = 0;
int frequency = 0;
int capCount = 0;

void Delay_ms(uint32_t time_ms);
void StartTimer(uint32_t time_ms);
uint64_t timerIsON();

uint32_t getInterruptedTime();
//If frequency ==TRUE, configure for frequency (keep trigger at 1/2*Vdd)
//If frequency == FALSE, configure for capacitance (change trigger to 0.63*Vdd)..also get comparator interrupt to return the time it took
void comparator_Init(bool mode)
{
    frequency = mode;
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
    
    
    TRISAbits.TRISA4 = 1; 	// A4 set as input on port pin
    AD1PCFGbits.PCFG3 = 0; 	// Set input to Analog
    
    IEC1bits.CMIE = 0; 		// IE Off so no interrupt occurs from setup
    CM1CONbits.COE = 0; 	// Disable output pin
    CM1CONbits.CPOL = 0; 	// Standard sense. +In High ==> Out High
    CM1CONbits.CREF = 1; 	// +IN is internal CVRef
    CM1CONbits.CCH = 1; 	// -IN is C1INB Pin
    //CM1CONbits.CON = 1; 	// Turn Comparator ON
    
    //change voltage reference based on mode 
    if(frequency){
        CVRCON = 0x88; 		// CVRef = (1/2) * (AVdd - AVss) 
        CM1CONbits.EVPOL = 2; 	// Event detected on output edge falling

    }
    else{ //for capacitance
        CVRCON = 0x8C; 		// CVRef = (0.6253) * (AVdd - AVss) //sets CVR<3:0> to 12
        CM1CONbits.EVPOL = 2; 	// Event detected on rising edge 

    }
    
    CM1CONbits.CEVT = 0;
    IFS1bits.CMIF = 0; 		// Clear IF after set-up

    // When C1INB RISES above AVdd/2,
    // C1OUT falls & edge is counted.
       
    IEC1bits.CMIE = 1; 
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void)
{
    IFS1bits.CMIF = 0;		// clear IF flag

    if (!frequency){ //this means we are measuring capacitance
        // emmanuels code to record the time and calculate it 
        //also display it here for simplicity
        //capCount++;
        if (CM1CONbits.CEVT == 1 ){ 	// Check C1EVT bit or CEVT
            Disp2String("ComparatorTriggered\n");
            CM1CONbits.CEVT = 0;
            capCount =0;
        }
    }
    
    if (frequency){ //for measuring frequency
    eventCount++; 
    }
    Nop();
    
    return;
}

double compare()
{
    if (!timerIsON())
    {
        StartTimer(1000);
        currentFrequency = eventCount;
        eventCount = 0;
    }
    
    return currentFrequency;
}*/
