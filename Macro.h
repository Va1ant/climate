#define pinAsOutput(pin) DDRB |= (1 << pin)
#define pinAsInput(pin)  DDRB &= ~(1 << pin)
#define pinToLOW(pin)    PORTB &=  ~(1 << pin)
#define pinToHIGH(pin)   PORTB |= (1 << pin)
#define readPin(pin)     ((PINB & (1 << pin)) ? 1 : 0)

#define HEATER_PIN PB3
#define TERMO_PIN PB4
#define ERROR_PIN PB2

#define DO_ERROR {\
    pinToLOW(HEATER_PIN);\
    pinAsOutput(ERROR_PIN);\
    pinToHIGH(ERROR_PIN);\
    continue;\
  }

// https://bitbucket.org/alxarduino/leshakutils
// v0.1
// details: http://alxarduino.blogspot.com/2013/09/ComfortablyBlinkWithoutDelay.html


// private, do not call direct
#define DO_EVERY_V(varname,interval,action) \
  static unsigned long __lastDoTime_##varname = 0; \
  if( (millis()-__lastDoTime_##varname>=interval )){ \
    {action;}\
    __lastDoTime_##varname=millis();\
  }\


#define DO_EVERY_V_1(varname,interval,action)  DO_EVERY_V(varname,interval,action) // required to resolve __LINE__

  // Public:
  // sample of use: DO_EVERY(1000,Serial.println("Print every second"))
  // more samples:

#define DO_EVERY(interval,action) DO_EVERY_V_1(__LINE__,interval,action)

  // converters to milliseconds

#define _SEC_(sec) 1000UL*sec
#define _MIN_(mn)  _SEC_(60*mn)
#define _HR_(h)  _MIN_(60*h)
#define _DAY_(d) _HR(24*d)
