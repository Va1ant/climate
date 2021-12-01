#include "main.h"

volatile uint8_t tim0_ovf = 0;

struct Relay heater = {
	.setpoint = 21 * 16,	// желаемая температура
	.hysteresis = 1 * 16,	// +- от желаемой
	.k = 1,					// коэффициент усиления обратной связи
	.prevInput = 0,
	.output = 0,
	.direction = 1
};

int main(void) {
	uint8_t time1;
	uint8_t data[9]; // [9] for crc; [7] if normal work; [2] for 0.5 temperature resolution
	
	pinAsOutput(ERROR_PIN);
	pinAsOutput(HEATER_PIN);
	
	// INT0
	pinAsInput(PB2);
	pinToHIGH(PB2);
	// data channel
	pinAsInput(PB0);
	pinToHIGH(PB0);
	
	// Timer0 init
	TCCR0B |= (1 << CS02 | 1 << CS00);	// clk/1024
	TIMSK |= (1 << TOIE0);	// TIM0 overflow interrup enable
	
	GIMSK |= (1 << INT0);	// Int0 init
	//MCUCR |= (1 << ISC01);	// maybe falling edge trigger better?
	
	sei();

	for (_Bool measure = 1;;) {
		if (measure) {
			if (ds_termo(0x44)) {	// compute command or termometer not on line
				time1 = tim0_ovf;
				measure = 0;
			}
		} else if ((uint8_t)(tim0_ovf - time1) >= 4) {	// about 1.048576 sec with 1024*256/1000000*4
			if (ds_termo(0xBE)) {	// read command or termometer not on line
				for (uint8_t i = 0; i < 9; i++) data[i] = ds_read(TERMO_PIN); // [9] for crc; [7] if normal work
				
				if (data[0] == 0xAA) DO_ERROR();	// if true, maybe problem with power on termometer
				if (ds_crc(data, 8) != data[8]) DO_ERROR();	// check crc; delete if work fine
				
				// calculate temperature and send it to relay regulator
				heater.input = ((data[0] << 3 | data[1] << 11) & 0xFFF0) + 12 - data[6];
				
				if (compute(&heater)) pinToHIGH(HEATER_PIN);
				else pinToLOW(HEATER_PIN);
				
				measure = 1;
			}
		} if (readPin(HEATER_PIN)) pinToLOW(ERROR_PIN);
	}
}

ISR (INT0_vect) {
	// data transfer led indicator 
	pinToHIGH(PB1);
	
	// debug delay
	_delay_loop_1(3);
	
	ds_write(heater.input >> 3, 1, PB0);
	ds_write(heater.setpoint >> 3, 1, PB0);
	heater.setpoint = ds_read(PB0) << 3;
	ds_write(heater.setpoint >> 3, 1, PB0);
	
	pinToLOW(PB1);
}

ISR (TIM0_OVF_vect) {
	tim0_ovf++;
}
