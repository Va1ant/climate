#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

// ERRSUM bits
#define TLGR 0	// Termometer Line Grounded
#define PORV 1	// Termometer Power On Reset Value
#define CRCW 2	// Termometer CRC Wrong
//define WDRF 3	// Watchdog Reset Flag from MCUSR
#define DLGR 4	// Data Line Grounded
#define ACTU 5	// Actual error

// scratchpad length
#define FULL	9
#define HIRES	7
#define LOWRES	2
#define NOSIGN	1
#define LENGTH	FULL

// scratchpad fields
#define TEMPERATURE	0
#define SIGN		1
#define REMAIN		6
#define CRC_VALUE	8

// saves the resolution of temperature and still operates with an integer
#define MULTIPLY(val) (val * 16)

// Register an error as actual & skip this iteration
#define errorReg(field) {\
	ERRSUM |= (1 << field) | (1 << ACTU);\
	pinToLow(HEATER_PIN);\
	pinToHigh(ERROR_PIN);\
	continue;\
}

#define _1SEC ((1 << WDP2) | (1 << WDP1))
#define _8SEC ((1 << WDP3) | (1 << WDP0))

volatile uint8_t ERRSUM;
volatile uint8_t now = 0;
uint8_t period;

inline void watchdog(const uint8_t mode, const uint8_t sec) {
	if (sec == _1SEC) period = 1;
	else if (sec == _8SEC) period = 8;
	
	asm ("wdr");
	WDTCR = (1 << WDCE) | (1 << WDE);
	WDTCR = (1 << mode) | sec;
}

struct relay heater = {
	.setpoint = MULTIPLY(21),
	.delta = MULTIPLY(1),
	.k = 1,
	.prevInput = 0,
	.output = 0,
	.prevTime = 0,
	.direction = 1
};

#endif
