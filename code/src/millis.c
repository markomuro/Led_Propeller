//https://github.com/monoclecat/avr-millis-function

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "millis.h"

ISR(TIMER3_COMPA_vect)
{
  timer3_millis++;
}

void init_millis(unsigned long f_cpu){
  unsigned long ctc_match_overflow;

  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer3 is this value, 1ms has passed

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR3B |= (1 << WGM32) | (1 << CS31);

  // high byte first, then low byte
  OCR3AH = (ctc_match_overflow >> 8);
  OCR3AL = ctc_match_overflow;

  // Enable the compare match interrupt
  TIMSK3 |= (1 << OCIE3A);
}

unsigned long millis (void){
  unsigned long millis_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer3_millis;
  }
  return millis_return;
}