#ifndef _LEDCLOCK_H_
#define _LEDCLOCK_H_

uint8_t hour;
uint8_t minute;
uint8_t second;
uint8_t t;
unsigned long previousTime;
uint8_t ledon_ticks;
uint16_t circle_length;
uint16_t sector_length;

typedef enum {
	IR_on,
	IR_off,
} IR_State;

typedef enum {
	isFirst,
	notFirst,
} Loop_Iter;

typedef enum {
	ledsOn,
	ledsOff,
} leds_State;

IR_State ir_led_state;
IR_State ir_led_state_prev;
Loop_Iter loop_iter_state;
leds_State hours_state;
leds_State minutes_state;
leds_State seconds_state;

ISR(TIMER1_COMPA_vect);
ISR(TIMER1_COMPB_vect);
ISR(TIMER1_COMPC_vect);


void ADC_init();
void update_time();
void setup_inital();
unsigned long millis (void);
void synchronize();
void set_OCR();


#endif