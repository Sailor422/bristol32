#pragma once
#include <Arduino.h>

class MmwaveGPIO {
  public:
    explicit MmwaveGPIO(uint8_t pin): pin_(pin) {}
    void begin() { pinMode(pin_, INPUT_PULLDOWN); }
    bool presence() const { return digitalRead(pin_); }
  private:
    uint8_t pin_;
};
