#ifndef Macro_h
#define Macro_h

#define pinAsOutput(pin) DDRB |= (1 << pin)
#define pinAsInput(pin)  DDRB &= ~(1 << pin)
#define pinToLOW(pin)    PORTB &=  ~(1 << pin)
#define pinToHIGH(pin)   PORTB |= (1 << pin)
#define readPin(pin)     ((PINB & (1 << pin)) ? 1 : 0)

#define HEATER_PIN PB3
#define TERMO_PIN PB4
//#define ERROR_PIN PB2

#define DO_ERROR(pin) {\
  pinToLOW(HEATER_PIN);\
  pinAsOutput(pin);\
  pinToHIGH(pin);\
  continue;\
}

#define HAPPY_END {\
  pinToLOW(PB0);\
  pinToLOW(PB1);\
  pinToLOW(PB2);\
}

#endif
