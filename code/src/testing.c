#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "testing.h"

void blink_led() {
	//blink led with delay
	PORTF ^= 1 << PORTF1;
	_delay_ms(1000);
}

void ir_test() {
	//turn on led when phototransistor passes IR led
	if (PINF & (1 << PINF0)){
		PORTF |= 1 << PORTF1;
	}
	else PORTF &= ~(1 << PORTF1);
}

void adc_test() {
	uint16_t adcvalue = ADC;
	if (adcvalue > 1010){
		PORTF |= 1 << PORTF1;
		PORTB = 0b01000000;
		} else {
		PORTF &= ~(1 << PORTF1);
		PORTB = 0x00;
	}
}

void two_clk_hands_static(){
	uint16_t adcvalue = ADC;
	if (adcvalue < 800) ir_led_state_prev = IR_off;
	if (ir_led_state_prev == IR_off && adcvalue > 1010){
		ir_led_state = IR_on;
		ir_led_state_prev = IR_on;
	}
	if (ir_led_state == IR_on){
		PORTF |= 1 << PORTF1;
		PORTB = 0b01000000;
		_delay_us(200);
		PORTF &= ~(1 << PORTF1);
		PORTB = 0x00;
		_delay_ms(10);
		PORTB = 0b10000000;
		_delay_us(200);
		PORTB = 0x00;
		ir_led_state = IR_off;
	}
}

void blink_led_ms(){
	if (millis() - prev_millis > 1000)
	{
		PORTF ^= 1 << PORTF1; //Turn on / Turn off the LED
		prev_millis = millis();
	}
}