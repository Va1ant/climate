#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

// ERRSUM bits
#define TLGR 0	// Termometer Line Grounded
#define PORV 1	// Termometer Power On Reset Value
#define CRCW 2	// Termometer CRC Wrong
//		WDRF 3	// Watchdog Reset Flag from MCUSR
#define DLGR 4	// Data Line Grounded
#define ACTU 5	// Actual error

// Watchdog timers
#define _1SEC ((1 << WDP2) | (1 << WDP1))
#define _8SEC ((1 << WDP3) | (1 << WDP0))

// avoid floating point calculations
#define MULTIPLY(val) (val * 16)

static volatile uint8_t ERRSUM;
static volatile uint8_t now = 0;
static uint8_t period;

struct relay heater = {
	.setpoint = MULTIPLY(21),
	.delta = MULTIPLY(1),
	.k = 1,
	.prevInput = 0,
	.prevTime = 0,
	.output = 0,
	.direction = 1
};

static void errorReg(const uint8_t field);
static void watchdog(const uint8_t mode, const uint8_t sec);

#endif
