#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "ledclock.h"

//variables for holding current values of hours, minutes and seconds
uint8_t hour = 10;
uint8_t minute = 15;
uint8_t second = 30;
//t and previousTime variables used for counting seconds in update_time()
uint8_t t = 0;
unsigned long previousTime = 0;
//value to add to Output Compare Register when seconds/minutes/hours LEDs are turned on
uint8_t ledon_ticks = 50; // 200us is fine -> 1 / (16MHz/64prescaler) -> 4us is one tick
//variable holding state of detection of IR LED
IR_State ir_led_state_prev = IR_off;
//variable for checking whether it's first loop iteration
Loop_Iter loop_iter_state = isFirst;
//current states of seconds/minutes/hours LEDs
leds_State hours_state = ledsOff;
leds_State minutes_state = ledsOff;
leds_State seconds_state = ledsOff;

void ADC_init(){
	ADCSRA=
	(1<<ADEN)|//converter enable
	(1<<ADSC)|//start conversion
	(1<<ADATE)| //auto trigger free running mode
	(1<<ADPS1)|(1<<ADPS2); // ADC prescaler division factor 64
	ADMUX=
	(1<<REFS0);//AVCC with external capacitor on AREF pin
}

//function for keeping track and updating time values
void update_time(){
	if (millis() >= previousTime){
		previousTime = previousTime + 25;
		t++;
		if (t == 40){
			t = 0;
			second++;
		}
	}
	if(second == 60){
		second = 0;
		minute++;
	}
	if(minute == 60){
		minute = 0;
		hour++;
	}
	if(hour > 11) hour = hour - 12;
}

void setup_inital() {
	//clock speed 16MHz
	CLKPR = (1<<CLKPCE);
	CLKPR = (0<<CLKPS3)|(0<<CLKPS2)|(0<<CLKPS1)|(0<<CLKPS0);
	
	MCUCR = (1<<JTD); //JTAG off so PORTF pins can be used as IO
	MCUCR = (1<<JTD);
	DDRF = 0xF2; // set PORTF pins as output or input
	PORTF = 0x02; // set 4 pins low for p-mosfet gates (to activate them)
	DDRD |= 0xF0; // set PORTD pins 4,5,6,7 as output
	PORTD = 0x00;//set 4 pins low for p-mosfets (to activate them)
	DDRB |= 0xE0;// set PORTB pins 5,6,7 as output (controlling N-ch mosfets)
	PORTB = 0x00; // set PORTB low
}

void synchronize(){
	uint16_t adcvalue = ADC;
	if (adcvalue < 800) ir_led_state_prev = IR_off;
	if (ir_led_state_prev == IR_off && adcvalue > 1010){
		ir_led_state = IR_on;
		ir_led_state_prev = IR_on;
	}
}

void set_OCR() {
	if (hour != 0) {
		OCR1A = sector_length * hour * 5;
		} else {
		hours_state = ledsOn;
		PORTB |= (1 << PORTB7);
		OCR1A = 50;
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
}

//when interrupt happens, then checking whether hours/minutes/seconds LEDs are already on or off
//if they were off, setting them on and adding appropriate value to OCR to ensure that LEDs are on for enough time for humans to see entire picture as clock
//if they were on, then turning them off and disabling the interrupt
ISR(TIMER1_COMPA_vect)
{
	if (hours_state == ledsOff) {
		hours_state = ledsOn;
		PORTD |= (1 << PORTD7);
		PORTB |= (1 << PORTB7);
		OCR1A += 50;
		} else {
		hours_state = ledsOff;
		PORTD &= ~(1 << PORTD7);
		PORTB &= ~(1 << PORTB7);
		TIMSK1 &= ~(1 << OCIE1A);
	}
}

ISR(TIMER1_COMPB_vect)
{
	if (minutes_state == ledsOff) {
		minutes_state = ledsOn;
		PORTB |= (1 << PORTB6);
		OCR1B += 50;
		} else {
		minutes_state = ledsOff;
		PORTB &= ~(1 << PORTB6);
		TIMSK1 &= ~(1 << OCIE1B);
	}
}

ISR(TIMER1_COMPC_vect)
{
	if (seconds_state == ledsOff) {
		seconds_state = ledsOn;
		PORTB |= (1 << PORTB5);
		OCR1C += 50;
		} else {
		seconds_state = ledsOff;
		PORTB &= ~(1 << PORTB5);
		TIMSK1 &= ~(1 << OCIE1C);
	}
}