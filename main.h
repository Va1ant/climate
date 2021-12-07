#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 1000000UL

#define LIGR 0	// Line Grounded
#define PORV 1	// Power On Reset Value
#define CRCW 2	// CRC Wrong

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

#endif
