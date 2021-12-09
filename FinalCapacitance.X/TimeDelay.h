/* 
 * File:   TimeDelay.h
 * Author: randy
 *
 * Created on October 15, 2021, 3:27 PM
 */

/*
 * 
 */

void Delay_ms(uint32_t time_ms);
void StartTimer(uint32_t time_ms);
void configTimers();
void configTimerInterrupt();
void NewClk(unsigned int clkval);
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void); //Interrupt for Timer2
