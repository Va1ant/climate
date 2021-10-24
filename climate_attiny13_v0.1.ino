#define F_CPU 1200000UL

#include <avr/io.h>
#include <util/delay.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

int main(void) {
  uint8_t data[9]; //[9] for crc; [7] if normal work
  GyverRelay heater;

  heater.setpoint = 21 * 16;  // желаемая температура
  heater.hysteresis = 1 * 16; // +- от желаемой
  heater.k = 1;               // коэффициент обратной связи

  pinAsOutput(HEATER_PIN);
  pinToLOW(HEATER_PIN);

  while (1) {
    if (!ds_reset()) DO_ERROR(PB2);
    ds_write(0xCC);
    ds_write(0x44); //make (0x44, 1) if parasite power, but be carefull, look at line 33 this file

    _delay_ms(1000);

    if (!ds_reset()) DO_ERROR(PB2);
    ds_write(0xCC);
    ds_write(0xBE);

    for (uint8_t i = 0; i < 9; i++) data[i] = ds_read(); //[9] for crc; [7] if normal work

    if (data[0] == 0xAA) DO_ERROR(PB1); // if true, maybe problem with power on termometer
    if (ds_crc8(data, 8) != data[8]) DO_ERROR(PB0); //check crc; delete this if normal work

    heater.input = ((data[0] << 3 | data[1] << 11) & 0xFFF0) + 12 - data[6];

    if (heater.compute()) pinToHIGH(HEATER_PIN);
    else pinToLOW(HEATER_PIN);

    HAPPY_END;
  }
}
