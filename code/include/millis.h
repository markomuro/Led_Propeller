#ifndef _MILLIS_H_
#define _MILLIS_H_

volatile unsigned long timer3_millis;

ISR(TIMER3_COMPA_vect);
void init_millis(unsigned long f_cpu);
unsigned long millis (void);

#endif