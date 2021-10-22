class GyverRelay {
  public:
    int16_t setpoint = 0;   // заданная величина, которую должен поддерживать регулятор (температура)
    int16_t hysteresis = 0; // +- от желаемой
    int16_t k = 0;          // коэффициент усиления по скорости
    int16_t input = 0;      // сигнал с датчика (например температура, которую мы регулируем)
    bool output = 0;        // выход регулятора

    bool compute();
  private:
    int16_t prevInput = 0;
    bool _direction = true;
};

int8_t signum(const int16_t val) {
  return ((val > 0) ? 1 : ((val < 0) ? -1 : 0));
}

boolean GyverRelay::compute() {
  int16_t signal;
  int16_t rate = input - prevInput;    // производная от величины (величина/секунду)
  prevInput = input;
  signal = input + rate * k;

  int8_t F = (signum(signal - setpoint - hysteresis) + signum(signal - setpoint + hysteresis)) >> 1;

  if (F == 1) output = !_direction;
  else if (F == -1) output = _direction;
  return output;
}
