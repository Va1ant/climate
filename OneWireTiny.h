#ifndef ONEWIRETINY_H_
#define ONEWIRETINY_H_

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include "Macro.h"

// Perform the onewire reset function.  We will wait up to 250uS for
// the bus to come high, if it doesn't then it is broken or shorted
// and we return a 0;
//
// Returns 1 if a device asserted a presence pulse, 0 otherwise.
//
_Bool ds_reset() {
	uint8_t retries = 125;

	pinAsInput(TERMO_PIN);
	// wait until the wire is high... just in case
	do {
		if (--retries == 0) return 0;
		asm ("nop");
	} while (!readPin(TERMO_PIN));

	// - Drop line
	pinToLOW(TERMO_PIN);
	pinAsOutput(TERMO_PIN);
	_delay_us(480);

	// - Listen for reply pulse
	pinAsInput(TERMO_PIN);
	_delay_us(70);

	// - Read line state
	_Bool state = !readPin(TERMO_PIN);
	_delay_us(410);
	return state;
}

//! Write single bit
void ds_writeBit(const _Bool Bit, const uint8_t pin) {
	// - Drop line
	pinToLOW(pin);
	pinAsOutput(pin);
	if (Bit & 1) {
		// - Write Bit-1
		pinToHIGH(pin);
		_delay_us(55);
	} else {
		// - Write Bit-0
		_delay_us(57);
		pinToHIGH(pin);
	}
}

//! Read single bit
_Bool ds_readBit(const uint8_t pin) {
	_Bool Bit;
	// - Drop line
	pinAsOutput(pin);
	pinToLOW(pin);
	_delay_loop_1(1);
	
	// - Wait for data
	pinAsInput(pin);
	_delay_loop_1(3);
	
	// - Read bit into byte
	Bit = readPin(pin);
	// let pin float, pull up will raise, говорили они
//	pinToHIGH(pin);
	
	_delay_us(45);
	return Bit;
}

//! Write byte
void ds_write(const uint8_t Byte, const _Bool Power, const uint8_t pin) {
	cli();
	// - Write each bit
	for (uint8_t BitMask = 0x01; BitMask; BitMask <<= 1) ds_writeBit(((BitMask & Byte) ? 1 : 0), pin);
	sei();
	// - Disable power
	if (!Power) {
		pinAsInput(pin);
		pinToLOW(pin);
	}
}

//! Read byte
uint8_t ds_read(const uint8_t pin) {
	uint8_t Byte = 0;
	cli();
	// - Read all bits
	for (uint8_t BitMask = 0x01; BitMask; BitMask <<= 1) {
		// - Read & store bit into byte
		if (ds_readBit(pin)) Byte |= BitMask;
	}
	sei();
	return Byte;
}

//! crc check
inline uint8_t ds_crc(const uint8_t *addr, uint8_t len) {
	uint8_t crc = 0;

	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}

inline _Bool ds_termo(const uint8_t cmd) {
	if (!ds_reset()) return 0;
	ds_write(0xCC, 0, TERMO_PIN);
	ds_write(cmd, 0, TERMO_PIN);
	return 1;
}

#endif
