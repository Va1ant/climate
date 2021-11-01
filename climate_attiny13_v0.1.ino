#define F_CPU 1200000UL

#include <avr/io.h>
#include <util/delay.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

int main(void) {
  uint8_t data[9]; //[9] for crc; [7] if normal work; [2] for 0.5 temperature resolution

  struct Relay heater;

  heater.setpoint = 21 * 16;  // желаемая температура
  heater.hysteresis = 1 * 16; // +- от желаемой
  heater.k = 1;               // коэффициент обратной связи
  heater.prevInput = 0;
  heater.output = 0;
  heater.direction = 1;

  pinAsOutput(HEATER_PIN);
  pinToLOW(HEATER_PIN);

  for (;;) {
    if (!ds_termo(0x44)) DO_ERROR(PB2); // compute command or termometer not on line

    _delay_ms(1000);                    // time to compute temperature

    if (!ds_termo(0xBE)) DO_ERROR(PB2); // read command or termometer not on line

    for (uint8_t i = 0; i < 9; i++) data[i] = ds_read(); //[9] for crc; [7] if normal work

    if (data[0] == 0xAA) DO_ERROR(PB1); // if true, maybe problem with power on termometer
    if (ds_crc(data, 8) != data[8]) DO_ERROR(PB0); //check crc; delete if work fine

    // calculate temperature and send it to relay regulator
    heater.input = ((data[0] << 3 | data[1] << 11) & 0xFFF0) + 12 - data[6];

    if (compute(&heater)) pinToHIGH(HEATER_PIN);
    else pinToLOW(HEATER_PIN);

    HAPPY_END;
  }
}
