#ifndef ONEWIRETINY_H_
#define ONEWIRETINY_H_

#define F_CPU 1000000UL

// ds commands
#define MEASURE		0x44
#define SKIP_ROM	0xCC
#define READ_SCRATCHPAD	0xBE
// The power-on reset value of the temperature register is +85C
#define RESET_VALUE	0xAA

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

//_Bool dsReset(void);
void dsWrite(const uint8_t byte, const uint8_t pin);
uint8_t dsRead(const uint8_t pin);
uint8_t dsCrc(const uint8_t *addr, uint8_t len);
_Bool dsTermo(const uint8_t cmd);

#endif
