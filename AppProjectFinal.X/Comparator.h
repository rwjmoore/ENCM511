uint16_t timerIsON();
int getInterruptedTime();
void comparator_Init(uint16_t mode);
float compare();
void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void);
