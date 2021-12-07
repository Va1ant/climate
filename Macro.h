#ifndef MACRO_H_
#define MACRO_H_

#define pinAsOutput(pin)	DDRB |= (1 << pin)
#define pinAsInput(pin)		DDRB &= ~(1 << pin)
#define pinToLOW(pin)		PORTB &=  ~(1 << pin)
#define pinToHIGH(pin)		PORTB |= (1 << pin)
#define pinInvert(pin)		PORTB ^= (1 << pin)
#define readPin(pin)		((PINB & (1 << pin)) ? 1 : 0)

#define ERROR_PIN PB1
#define HEATER_PIN PB3
#define TERMO_PIN PB4

#define DO_ERROR(void) {\
	pinToLOW(HEATER_PIN);\
	pinToHIGH(ERROR_PIN);\
	continue;\
}

#endif
