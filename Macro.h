#ifndef MACRO_H_
#define MACRO_H_

#define pinAsOutput(pin)	DDRB |= (1 << pin)
#define pinAsInput(pin)		DDRB &= ~(1 << pin)
#define pinToLow(pin)		PORTB &=  ~(1 << pin)
#define pinToHigh(pin)		PORTB |= (1 << pin)
#define pinInvert(pin)		PORTB ^= (1 << pin)
#define readPin(pin)		((PINB & (1 << pin)) ? 1 : 0)

#define ERROR_PIN	PB1
#define DATA_PIN	PB2
#define HEATER_PIN	PB3
#define TERMO_PIN	PB4

#endif
