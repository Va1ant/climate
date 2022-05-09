#ifndef RELAYTINY_H_
#define RELAYTINY_H_

#include <stdint.h>
#include <avr/interrupt.h>

struct relay {
	volatile int16_t setpoint;
	volatile uint8_t delta;
	volatile int8_t k;	// input change rate coefficient
	
	int16_t input;
	int16_t prevInput;
	uint8_t prevTime;
	_Bool output;
	
	const _Bool direction;
};

_Bool compute(struct relay *r, uint8_t t) {
	int16_t rate = (r->input - r->prevInput) / (uint8_t)(t - r->prevTime);
	r->prevTime = t;
	r->prevInput = r->input;
	
	cli();
	int16_t signal = r->input + rate * r->k;

	if (signal < (r->setpoint - r->delta)) r->output = r->direction;
	else if (signal > (r->setpoint + r->delta)) r->output = !r->direction;
	sei();
	
	return r->output;
}

#endif
