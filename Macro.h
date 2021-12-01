#ifndef Macro_h
#define Macro_h

#define pinAsOutput(pin)	DDRB |= (1 << pin)
#define pinAsInput(pin)		DDRB &= ~(1 << pin)
#define pinToLOW(pin)		PORTB &=  ~(1 << pin)
#define pinToHIGH(pin)		PORTB |= (1 << pin)
#define invertPin(pin)		PORTB ^= (1 << pin)
#define readPin(pin)		((PINB & (1 << pin)) ? 1 : 0)

#define ERROR_PIN PB1
#define HEATER_PIN PB3
#define TERMO_PIN PB4

void DO_ERROR() {
	pinToLOW(HEATER_PIN);
	pinToHIGH(ERROR_PIN);
}

#endif
