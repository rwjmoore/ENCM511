/*
 * File:   newmainXC16.c
 * Author: randy
 *
 * Created on November 30, 2021, 3:58 PM
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
#include "CTMU.h"

//#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled 
//#pragma config OSCIOFNC = ON //CLKO output disabled on pin 8, use as IO.
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void); //CN Interrupt (for IO Change Notification)

int main(void){
    IOinit();
    NewClk(8);
    //CTMUinit();
    float cap = do_CTMU();
    //Delay_ms(1000);
    Disp2Dec(cap);
    Disp2String("\n");
    
    
    return 0;
}






//OLD STUFF!!!!!!!
//void CTMUsetup();
//
//int main(void){
//    //dont need A/D converter 
//    // dont need comparator 2 
//    NewClk(8);
//    CTMUsetup();
//    
//    
//    unsigned int Vread = 0;
//    unsigned int Vavg = 0;
//    unsigned int VTot = 0;
//    CTMUCONbits.CTMUEN = 0; //Enable the CTMU
//    int j = 0;
//    for(j=0;j<10;j++){
//        AD1CON1bits.SAMP = 1; //Manual sampling start
//        CTMUCONbits.IDISSEN = 1; //drain charge on the circuit
//        int i = 0;
//        DELAY; //wait 125us
//        CTMUCONbits.IDISSEN = 0; //end drain of circuit
//        CTMUCONbits.EDG1STAT = 1; //Begin charging the circuit
//        //using CTMU current source
//        DELAY; //wait for 125us
//        CTMUCONbits.EDG1STAT = 0; //Stop charging circuit
//        IFS0bits.AD1IF = 0; //make sure A/D Int not set
//        AD1CON1bits.SAMP = 0; //and begin A/D conv.
//        while(!IFS0bits.AD1IF); //Wait for A/D convert complete
//        AD1CON1bits.DONE = 0;
//        Vread = ADC1BUF0; //Get the value from the A/D
//        IFS0bits.AD1IF = 0; //Clear A/D Interrupt Flag
//        VTot += Vread;
//    }
//    Vavg = (VTot/j);
//    //now we calculate voltage from this reading, and then get capacitance
//    uint64_t sampleVoltage = (Vavg* 3.25 / 1023) * 1000;
////    char ADCoutput[6] = "";
////  sprintf(ADCoutput, "%PRIu64", sampleVoltage); //convert integer to string
//
//    Disp2Dec(sampleVoltage);
//    Disp2String("\r");
//    //Ctotal = I*T/V
//    /* I is in microamps, T in microseconds, V in microV */
////    uint64_t sampleVoltageMicro = sampleVoltage*1000.00;
////    float capacitance = 0.55/sampleVoltageMicro*125;
////    Disp2Dec(capacitance);
////    Disp2String("\r");
////    return 0;
//    
//}
//
//void CTMUsetup(){
//    //CTMU INITIALIZATION
//    CTMUICONbits.IRNG = 01; //Setting current range... 01 = 0.55 micro Amps
//    CTMUICONbits.ITRIM = 0; // Current trim bits: currently set to no change
//    
//    CTMUCONbits.EDG1SEL = 00; //this is how we select what triggers edge1 
//    CTMUCONbits.EDG2SEL = 00; //this is how we select whawt triggers edge2
//    
//    //configuring input polarities for the edges 
//    CTMUCONbits.EDG1POL = 1; //0 is  for negative edge response 
//    CTMUCONbits.EDG2POL = 1; 
//    
//    //Edge sequencing: if edge1 has to occur before edge 2
//    CTMUCONbits.EDGSEQEN = 0; //no edge sequence needed 
//    
//    //Time Generation Enable bit
//    CTMUCONbits.TGEN = 1; //This changes between measurement or time delay 
//    
//    CTMUCONbits.CTTRIG = 1; //this bit automatically triggers an A/D conversion ****
//    
//    //now we discharge the current ensure capacitor is discharged
//    CTMUCONbits.IDISSEN = 1; 
//    int i = 0; //had to use to accomodate the delay loop
//    DELAY;  //waits for 125us
//    CTMUCONbits.IDISSEN = 0; // end drain of circuit
//    
//    //Disable the module 
//    CTMUCONbits.CTMUEN = 0; 
//    
//    //Clear edge status bits
//    CTMUCONbits.EDG1STAT = 0; 
//    CTMUCONbits.EDG2STAT = 0; // these show if an edge response has occured. Controls the current source 
//    //output to external analog modules.CTMU automatically sets a particualr bit when an edge response is detected on its channel.  Current only supplied when EDG1STAT is not equal to EDG2STAT 
//    
//    //enable the edge bits
//    CTMUCONbits.EDGEN = 1; //makes the edge bits unblocked
//    
//    //now enable the module 
//    CTMUCONbits.CTMUEN = 1; 
//    //INITIALIZATION DONE
//    
//    /**************************************************************************/
//// Set up AD Converter *****************************************************/
///**************************************************************************/
//
//                                                //AD1CHS=0x003; // Select the analog channel(3)
//                                                //AD1CSSL=0x0000; // i think this turns off channels for input scan
//                                                //AD1CON1bits.ADON = 1; // Turn On A/D Converter, continue in Idle mode,
//                                                //// Unsigned fractional format, Clear SAMP bit to
//                                                //// start conversion, Sample when SAMP bit is set,
//                                                //// sampling on hold
//                                                //AD1CON2 = 0x0000; // VR+ = AVDD, V- = AVSS, Don't scan,
//                                                //// always use MUX A inputs
//                                                //AD1CON3 = 0x0000; // A/D uses system clock, conversion clock = 1xTcy
//AD1CON1bits.ADON=0;            //Turn off ADC, turn on before sampling in do_ADC()
//AD1CON1bits.ADSIDL=0;        //Continue ADC operation in idle mode
//AD1CON1bits.FORM=0b00;        //0b00: Int, 01: Signed Int, 10:Fractional, 11:Signed Fract
//AD1CON1bits.SSRC=0b000;    //Start ADC conversion Time set by SAMC has passed
//AD1CON1bits.ASAM=0;     //Next Sampling begins when SAMP bit is set
//AD1CON1bits.SAMP=0;        //Set this bit in ADC routine to start sampling  
//
//// Sampling time and ADC Clk select
//AD1CON2bits.VCFG=0b000;     //Vref+=AVDD   Vref-=AVSS
//AD1CON2bits.CSCNA=0;    //Do not scan inputs; use channel selected by CH0SA bits
//AD1CON2bits.SMPI=0b0000; //Any ADC interrrups occur at completion of each sample/convert seq.
//AD1CON2bits.BUFM=0; //Buffer confugured as 1 16 word buffer 
//AD1CON2bits.ALTS=0; //Always use MUXA input MUX settings
//AD1CON3bits.ADRC=0; //0: Use System clk; 1: ADC uses internal RC clock for idle or sleep mode
//AD1CON3bits.SAMC=0; 
//AD1CON3bits.ADCS=0; //ADCS conversion clock select bits. Ignored if using internal AD RC clock. 
//
//// MUX settings
//AD1CHSbits.CH0NB=0;    //Ch0 MUXB -ve Input is VR-
//AD1CHSbits.CH0SB=0b011;    //YOUR ADC CHANNEL HERE
//AD1CHSbits.CH0NA=0;    //Ch0 -ve Input is VR-
//AD1CHSbits.CH0SA=0b011;    //YOUR ADC CHANNEL HERE
//
////IO port selection
//TRISBbits.TRISB1=1; // Set channel 1 as an input
//AD1PCFGbits.PCFG3=1; //MAKE AN3 an analog input 
//
//AD1CON1bits.ADON=1;
//
////So here, our A/D converter is set to PIN 5********
//}