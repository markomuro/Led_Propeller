
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <millis.h>
#include <ledclock.h>


int main(void){
	//setting needed pins, initializing ADC and milliseconds counting timer, enabling interrupts
	setup_inital();
	ADC_init();
	init_millis(16000000UL);
	sei();
	
    while (1) {
		update_time();//updating time values
		uint16_t adcvalue = ADC; //read ADC value
		if (adcvalue < 800) ir_led_state_prev = IR_off; //if ADC values are low, then IR LED is not detected
		if (ir_led_state_prev == IR_off && adcvalue > 1010){ //synchronize frame - if ADC values are sufficiently high and previous state was 'off', then IR LED was detected; changing states accordingly
			ir_led_state = IR_on;
			ir_led_state_prev = IR_on;
		}
		if (ir_led_state == IR_on) { // if new frame started
			ir_led_state = IR_off; //changing the state to 'off' to ensure that we only get here once during one "rotation"
			if (loop_iter_state == isFirst) { // check for very first rotation
				TCNT1 = 0; //set timer register to zero
				TCCR1B |= (1 << CS11) | (1 << CS10); //start timer with clk/64 prescaler
				loop_iter_state = notFirst;
				continue;
			}
			circle_length = TCNT1; //get number of ticks for previous full rotation
			TCCR1B &= ~((1 << CS11) | (1 << CS10)); // timer/counter stop
			TCNT1 = 0; //clearing the timer register
			sector_length = (int) (circle_length / 60); //calculate sector length
			if (hour != 0) { // if hour hand is not pointing to 0/12
				OCR1A = sector_length * hour * 5; //set compare match register value for hour
				} else { // else if hour hand is at 12'o clock
				hours_state = ledsOn; 
				PORTB |= (1 << PORTB7); // turn on hour hand
				OCR1A = 50; // set compare match register value for hour hand delay
			}
			if (minute != 0) {
				OCR1B = sector_length * minute;
				} else {
				minutes_state = ledsOn;
				PORTB |= (1 << PORTB6);
				OCR1B = 50;
			}
			if (second != 0) {
				OCR1C = sector_length * second;
				} else {
				seconds_state = ledsOn;
				PORTB |= (1 << PORTB5);
				OCR1C = 50;
			}
			TIMSK1 |= (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A); // enable timer interrupts
			TCCR1B |= (1 << CS11) | (1 << CS10); //start timer with clk/64 prescaler
		}
		
    }
}

