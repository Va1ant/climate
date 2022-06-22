#include <avr/interrupt.h>
#include "RelayTiny.h"

_Bool compute(struct relay *r, uint8_t t) {
	int16_t rate = (r->input - r->prevInput) / (uint8_t)(t - r->prevTime);
	r->prevTime = t;
	r->prevInput = r->input;
	
	uint8_t cSREG = SREG;
	cli();
	int16_t signal = r->input + rate * r->k;
	
	if (signal < (r->setpoint - r->delta)) r->output = r->direction;
	else if (signal > (r->setpoint + r->delta)) r->output = !r->direction;
	SREG = cSREG;
	
	return r->output;
}
