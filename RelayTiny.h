#ifndef RELAYTINY_H_
#define RELAYTINY_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

struct Relay {
	volatile int16_t setpoint;	// заданная величина, которую должен поддерживать регулятор (температура)
	int16_t hysteresis;		// +- от желаемой
	int16_t k;			// коэффициент усиления обратной связи
	int16_t input;			// сигнал с датчика (например температура, которую мы регулируем)
	int16_t prevInput;
	_Bool output;			// выход регулятора
	const _Bool direction;		// направление регулятора
	
	uint8_t prevTime;
};

inline int8_t signum(const int16_t val) {
	return ((val > 0) ? 1 : ((val < 0) ? -1 : 0));
}

_Bool compute(struct Relay *r, uint8_t t) {
	int16_t signal;
	int16_t rate = (r->input - r->prevInput) / (uint8_t)(t - r->prevTime);	// производная от величины (величина/время)
	r->prevTime = t;
	r->prevInput = r->input;
	//cli(); // if volatile k
	signal = r->input + rate * r->k;
	
	cli();
	int8_t F = (signum(signal - r->setpoint - r->hysteresis) + signum(signal - r->setpoint + r->hysteresis)) >> 1;
	sei();
	
	if (F == 1) r->output = !r->direction;
	else if (F == -1) r->output = r->direction;
	return r->output;
}

#endif
