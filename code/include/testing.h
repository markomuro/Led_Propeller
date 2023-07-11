#ifndef _TESTING_H_
#define _TESTING_H_

typedef enum {
	IR_on,
	IR_off,
} IR_State;

IR_State ir_led_state;
IR_State ir_led_state_prev;

unsigned long millis (void);
unsigned long prev_millis; //the last time stamp

void blink_led();
void ir_test();
void adc_test();
void two_clk_hands_static();
void blink_led_ms();

#endif