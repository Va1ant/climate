#include "main.h"

volatile uint8_t seconds = 0;
uint8_t period;
uint8_t ERRSUM;

struct Relay heater = {	// default values for:
	.setpoint = 21 * 16,	// желаемая температура
	.hysteresis = 1 * 16,	// +- от желаемой
	.k = 1,			// коэффициент усиления обратной связи
	.prevInput = 0,
	.output = 0,
	.direction = 1
};

int main(void) {
	uint8_t time1;
	uint8_t data[9];	// [9] for crc; [7] if normal work; [2] for 0.5 temperature resolution
	
	/* Power Reduction config */
	ACSR = (1 << ACD);	// Analog comparator disable
	PRR = (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRUSI) | (1 << PRADC);	// timer1, timer0, USI, ADC disable
	
	/* Pin config */
	pinAsOutput(ERROR_PIN);	// actual error led indication
	pinAsOutput(HEATER_PIN);
	pinToHIGH(PB2);			// pullup INT0
	pinToHIGH(PB0);			// pullup data line
	
	/* Interrupt config */
	GIMSK = (1 << INT0);
	sei();

	for (_Bool measure = 1;;) {
		if (measure) {
			if(ds_termo(0x44)) {	// compute command
				time1 = seconds;
				measure = 0;
				
				/* Watchdog config */
				//WDTCR = (1 << WDCE) | (1 << WDE);	// без этой строчки не работает симуляция в proteus
				WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1); // 1 sec
				period = 1;
			} else {
				ERRSUM |= (1 << LIGR);
				DO_ERROR();	// skip iteration
			}
		} else if ((uint8_t)(seconds - time1) >= 1) {
			measure = 1;	// measure in next iteration
			if(ds_termo(0xBE)) {	// read scratchpad command
				for (uint8_t i = 0; i < 9; i++) data[i] = ds_read(TERMO_PIN); // read termometer scratchpad into data[]
				
				if (data[0] == 0xAA) {	// AA is termometer power-on reset value. If true, maybe problem with power on termometer
					ERRSUM |= (1 << PORV);
					DO_ERROR();
				}
				if (ds_crc(data, 8) != data[8]) {	// check crc
					ERRSUM |= (1 << CRCW);
					DO_ERROR();
				}
				pinToLOW(ERROR_PIN);	// no errors in this iteration
				
				cli();
				// calculate temperature and send it to relay regulator
				heater.input = ((data[0] << 3 | data[1] << 11) & 0xFFF0) + 12 - data[6];
				sei();
				
				if (compute(&heater, seconds)) {
					pinToHIGH(HEATER_PIN);
					continue;	// skip sleep instruction. will sleep after sending compute cmd
				} else {
					pinToLOW(HEATER_PIN);
					
					/* Watchdog config */
					//WDTCR = (1 << WDCE) | (1 << WDE);	// без этой строчки не работает симуляция в proteus
					WDTCR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); // 8 sec
					period = 8;
				}
			} else {
				ERRSUM |= (1 << LIGR);
				DO_ERROR();	// skip iteration
			}
		}
		// no errors in this iteration. Go to sleep
		/* Sleep config */
		MCUCR = (1 << SM1) | (1 << SE); // Power-down mode
		asm ("sleep");
		MCUCR &= ~(1 << SE);
	} // for
} // main

ISR (WDT_vect) {
	seconds += period;
}

ISR (INT0_vect) {
	// "sync" delay
	_delay_loop_1(3);
	
	ds_write(ERRSUM, 1, PB0);
	ds_write(heater.input >> 3, 1, PB0);
	ds_write(heater.setpoint >> 3, 1, PB0);
	heater.setpoint = (ds_read(PB0) << 3);
}
