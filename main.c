#include "main.h"

int main(void) {
	uint8_t someTime;
	uint8_t scratchpad[LENGTH];
	
	if (MCUSR & (1 << WDRF)) ERRSUM |= (1 << WDRF);
	
	// Power Reduction config
	ACSR = (1 << ACD);
	PRR = (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRUSI) | (1 << PRADC);
	
	// Pin config
	DDRB = (1 << ERROR_PIN) | (1 << HEATER_PIN);
	
	// Interrupt config
	GIMSK = (1 << INT0);
	sei();

	for (_Bool measure = 1;;) {
		if (measure) {
			if(dsTermo(MEASURE)) {
				someTime = now;
				measure = 0;
				watchdog(WDIE, _1SEC);
			} else errorReg(TLGR);
		} else if ((uint8_t)(now - someTime) >= 1) {
			measure = 1;
			if(dsTermo(READ_SCRATCHPAD)) {
				for (uint8_t i = 0; i < LENGTH; i++) scratchpad[i] = dsRead(TERMO_PIN);
				
				if (scratchpad[TEMPERATURE] == RESET_VALUE)	// maybe problem with power on termometer
					errorReg(PORV);
				if (dsCrc(scratchpad, FULL-1) != scratchpad[CRC_VALUE])
					errorReg(CRCW);

				pinToLow(ERROR_PIN);	// no errors in this iteration
				ERRSUM &= ~(1 << ACTU);
				
				// put the pieces of temperature value together and send it to relay regulator
				int16_t dTemp = (scratchpad[SIGN] << 8) | scratchpad[TEMPERATURE];	// doubled signed temperature value, directly from termometer
				cli();
				heater.input = ((dTemp << 3) & 0xFFF0) + 12 - scratchpad[REMAIN];	// high-precision temperatire multiplied by 16, to operate with an integer
				sei();
				
				if (compute(&heater, now)) {
					pinToHigh(HEATER_PIN);
					continue;	// skip sleep instruction while heating
				} else {
					pinToLow(HEATER_PIN);
					watchdog(WDIE, _8SEC);
				}
			} else errorReg(TLGR);
		} if (!period) watchdog(WDIE, _1SEC);
		// Sleep config
		MCUCR = (1 << SM1) | (1 << SE); // Power-down mode; sleep enable
		asm ("sleep");
		MCUCR &= ~(1 << SE);			// Sleep disable, as datasheet recomends
	} // for
} // main

ISR (WDT_vect) {
	now += period;
	// Reconfigure wdt to reboot in 8 sec to handle system hang-ups, just in case
	watchdog(WDE, _8SEC);
}

ISR (INT0_vect) {
	GIMSK &= ~(1 << INT0);
	
//	if (period == 8) now++;
	
	uint8_t retries = 100;
	do {
		if (--retries == 0) {
			ERRSUM |= (1 << DLGR);
			GIMSK |= (1 << INT0);
			return;
		}
	} while (!readPin(DATA_PIN));
	
	switch (dsRead(DATA_PIN)) {
		case 0x01:	// send all data
			dsWrite(ERRSUM, DATA_PIN);
			dsWrite(heater.input >> 2, DATA_PIN);
			dsWrite(heater.setpoint >> 2, DATA_PIN);
			dsWrite(heater.delta, DATA_PIN);
			dsWrite(heater.k, DATA_PIN);
		break;
		case 0x02:	// recieve new presets
			heater.setpoint = (dsRead(DATA_PIN) << 2);
			heater.delta = dsRead(DATA_PIN);
			heater.k = dsRead(DATA_PIN);
		break;
		case 0x03:	// clear errsum
			ERRSUM = 0;
		break;
		case 0x00:	// line grounded
			ERRSUM |= (1 << DLGR);
		break;
		case 0xFF:	// no command recieved
		break;
		default:	// unknown command
			dsWrite(0xAA, DATA_PIN);
	}
	GIMSK |= (1 << INT0);
}
