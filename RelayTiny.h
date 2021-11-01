#include <stdint.h>
#include <stdbool.h>

struct Relay {
	int16_t setpoint;   // заданная величина, которую должен поддерживать регулятор (температура)
	int16_t hysteresis; // +- от желаемой
	int16_t k;          // коэффициент усиления по скорости
	int16_t input;      // сигнал с датчика (например температура, которую мы регулируем)
	int16_t prevInput;
	_Bool output;       // выход регулятора
	_Bool direction;
};

int8_t signum(const int16_t val) {
	return ((val > 0) ? 1 : ((val < 0) ? -1 : 0));
}

inline _Bool compute(struct Relay *r) {
	int16_t signal;
	int16_t rate = r->input - r->prevInput;    // производная от величины (величина/секунду)
	r->prevInput = r->input;
	signal = r->input + rate * r->k;

	int8_t F = (signum(signal - r->setpoint - r->hysteresis) + signum(signal - r->setpoint + r->hysteresis)) >> 1;

	if (F == 1) r->output = !r->direction;
	else if (F == -1) r->output = r->direction;
	return r->output;
}
