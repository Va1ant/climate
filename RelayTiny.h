#ifndef RELAYTINY_H_
#define RELAYTINY_H_

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

_Bool compute(struct relay *r, uint8_t t);

#endif
