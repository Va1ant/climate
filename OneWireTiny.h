// maybe timing should be corrected. look datasheet

#include <stdint.h>
#include <stdbool.h>
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
    _delay_us(2);
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
void ds_writeBit(const _Bool Bit) {
  if (Bit & 1) {
    // - Drop line
    pinToLOW(TERMO_PIN);
    pinAsOutput(TERMO_PIN);
    // - Write Bit-1
    _delay_us(10);
    pinToHIGH(TERMO_PIN);
    _delay_us(55);
  } else {
    // - Drop line
    pinToLOW(TERMO_PIN);
    pinAsOutput(TERMO_PIN);
    // - Write Bit-0
    _delay_us(65);
    pinToHIGH(TERMO_PIN);
    _delay_us(5);
  }
}

//! Read single bit
_Bool ds_readBit() {
  // - Drop line
  pinAsOutput(TERMO_PIN);
  pinToLOW(TERMO_PIN);
  _delay_us(3);

  // - Wait for data
  pinAsInput(TERMO_PIN);
  _delay_us(10);

  // - Read bit into byte
  _Bool Bit = readPin(TERMO_PIN);
  _delay_us(53);
  return Bit;
}

//! Write byte
void ds_write(const uint8_t Byte, _Bool Power) {

  // - Write each bit
  for (uint8_t BitMask = 0x01; BitMask; BitMask <<= 1) ds_writeBit((BitMask & Byte) ? 1 : 0);

  // - Disable power
  if (!Power) {
    pinAsInput(TERMO_PIN);
    pinToLOW(TERMO_PIN);
  }
}

//! Read byte
uint8_t ds_read() {
  uint8_t Byte = 0;

  // - Read all bits
  for (uint8_t BitMask = 0x01; BitMask; BitMask <<= 1) {
    // - Read & store bit into byte
    if (ds_readBit()) Byte |= BitMask;
  }
  return Byte;
}

//crc check; delete if fine work
uint8_t ds_crc(const uint8_t *addr, uint8_t len) {
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

_Bool ds_termo(const uint8_t cmd) {
  if (!ds_reset()) return 0;
  ds_write(0xCC, 0);
  ds_write(cmd, 0);
  return 1;
}
