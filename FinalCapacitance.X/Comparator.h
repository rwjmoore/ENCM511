#include <xc.h> // include processor files - each processor file is guarded.

void comparator_Init(int mode);
double compare();
void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void);