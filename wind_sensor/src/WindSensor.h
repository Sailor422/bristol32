#ifndef WIND_SENSOR_H
#define WIND_SENSOR_H

#include <Arduino.h>

class WindSensor {
public:
  // Config
  static const int PIN_ANEMOMETER = 34; // Analog Input for Speed
  static const int PIN_VANE = 36;       // Analog Input for Direction (Changed from 35 to avoid LoRa conflict)

  // Calibration constants (based on design doc)
  // Anemometer output: 0.4V (0 m/s) to 2.0V (50 m/s)
  // ESP32 ADC: 0-3.3V -> 0-4095
  static constexpr float VOLT_MIN = 0.4;
  static constexpr float VOLT_MAX = 2.0;
  static constexpr float SPEED_MAX_MS = 50.0;

  void begin() {
    analogReadResolution(12);
    pinMode(PIN_ANEMOMETER, INPUT);
    pinMode(PIN_VANE, INPUT);
  }

  // Returns wind speed in mm/s
  uint16_t getWindSpeed() {
    // Read voltage (average of 10 samples)
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += analogRead(PIN_ANEMOMETER);
      delay(1);
    }
    float voltage = (sum / 10.0) * (3.3 / 4095.0);

    if (voltage <= VOLT_MIN)
      return 0;

    float speed_ms =
        (voltage - VOLT_MIN) * (SPEED_MAX_MS / (VOLT_MAX - VOLT_MIN));
    if (speed_ms < 0)
      speed_ms = 0;

    return (uint16_t)(speed_ms * 1000); // Convert to mm/s
  }

  // Returns wind direction in degrees * 10 (0-3599)
  uint16_t getWindDirection() {
    // Read voltage
    uint16_t reading = analogRead(PIN_VANE);
    // 0-3.3V maps to 0-360 degrees
    // ADC 0-4095 -> 0-360
    // Simple linear mapping for now (assuming pot is linear and no dead zone)
    // Design doc mentions 16 positions, but if it's a continuous pot:

    float degrees = (reading / 4095.0) * 360.0;
    return (uint16_t)(degrees * 10);
  }
};

#endif
