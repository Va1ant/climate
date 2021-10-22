/*
  - переписать программу без delay()
  - макрос для общения с датчиком?
  - допилить обработчик ошибок
*/

#define F_CPU 1200000UL

#include <avr/io.h>
#include <util/delay.h>
#include "Macro.h"
#include "OneWireTiny.h"
#include "RelayTiny.h"

int main(void) {
  uint8_t data[7];
  GyverRelay heater;

  heater.setpoint = 21 * 16;  // желаемая температура
  heater.hysteresis = 1 * 16; // +- от желаемой
  heater.k = 1;               // коэффициент обратной связи

  pinAsOutput(HEATER_PIN);
  pinToLOW(HEATER_PIN);

  while (1) {
    if (!ds_reset()) DO_ERROR;
    ds_write(0xCC);
    ds_write(0x44, 1);

    _delay_ms(1500);

    if (!ds_reset()) DO_ERROR;
    ds_write(0xCC);
    ds_write(0xBE);

    for (uint8_t i = 0; i < 7; i++) data[i] = ds_read();

    heater.input = ((data[0] << 3 | data[1] << 11) & 0xFFF0) + 12 - data[6];

    if (heater.input == 0 || heater.input == 85 * 16) DO_ERROR;

    if (heater.compute()) pinToHIGH(HEATER_PIN);
    else pinToLOW(HEATER_PIN);
    
    pinToLOW(ERROR_PIN);
  }
}
