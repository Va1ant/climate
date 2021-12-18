#ifndef RELAYTINY_H_
#define RELAYTINY_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

struct relay {
	volatile int16_t setpoint;
	volatile int16_t delta;
	volatile int16_t k;	// input change rate coefficient
	
	int16_t input;
	int16_t prevInput;
	uint8_t prevTime;
	_Bool output;
	
	const _Bool direction;
};

inline int8_t signum(const int16_t val) {
	return ((val > 0) ? 1 : ((val < 0) ? -1 : 0));
}

_Bool compute(struct relay *r, uint8_t t) {
	int16_t rate = (r->input - r->prevInput) / (uint8_t)(t - r->prevTime);
	
	cli();
	int16_t signal = r->input + rate * r->k;
	int8_t sign = (signum(signal - r->setpoint - r->delta) + signum(signal - r->setpoint + r->delta)) >> 1;
	sei();
	
	r->prevTime = t;
	r->prevInput = r->input;
	
	if (sign == 1) r->output = !r->direction;
	else if (sign == -1) r->output = r->direction;
	return r->output;
}

#endif
