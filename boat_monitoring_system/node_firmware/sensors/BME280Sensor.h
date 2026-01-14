#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include "SensorBase.h"
#include <Adafruit_BME280.h>

/**
 * BME280 Environmental Sensor
 * Measures temperature, humidity, and barometric pressure
 */
class BME280Sensor : public SensorBase {
private:
  Adafruit_BME280 bme;
  float temperature;     // Celsius
  float humidity;        // Percentage
  float pressure;        // hPa
  bool initialized;

  // For trend calculation
  static const int HISTORY_SIZE = 12; // 1 hour at 5-minute intervals
  float pressureHistory[HISTORY_SIZE];
  int historyIndex;
  unsigned long lastHistoryUpdate;

public:
  BME280Sensor() : temperature(0), humidity(0), pressure(0),
                   initialized(false), historyIndex(0),
                   lastHistoryUpdate(0) {
    memset(pressureHistory, 0, sizeof(pressureHistory));
  }

  bool begin() override {
    // Try I2C address 0x76 first, then 0x77
    initialized = bme.begin(0x76);
    if (!initialized) {
      initialized = bme.begin(0x77);
    }

    if (initialized) {
      // Configure sensor for weather monitoring
      bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                     Adafruit_BME280::SAMPLING_X2,  // temperature
                     Adafruit_BME280::SAMPLING_X16, // pressure
                     Adafruit_BME280::SAMPLING_X1,  // humidity
                     Adafruit_BME280::FILTER_X16,
                     Adafruit_BME280::STANDBY_MS_1000);

      // Initial reading
      read();
    }

    return initialized;
  }

  bool read() override {
    if (!initialized) return false;

    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F; // Pa to hPa

    // Update pressure history every 5 minutes
    unsigned long now = millis();
    if (now - lastHistoryUpdate >= 300000) { // 5 minutes
      pressureHistory[historyIndex] = pressure;
      historyIndex = (historyIndex + 1) % HISTORY_SIZE;
      lastHistoryUpdate = now;
    }

    // Check for valid readings
    if (isnan(temperature) || isnan(humidity) || isnan(pressure)) {
      return false;
    }

    return true;
  }

  bool isAvailable() override {
    return initialized && (bme.sensorID() == 0x60); // BME280 chip ID
  }

  String getStatusString() override {
    if (!initialized) return "BME280: Not initialized";

    char buf[64];
    snprintf(buf, sizeof(buf), "%.1fC %.0f%% %.0fhPa",
             temperature, humidity, pressure);
    return String(buf);
  }

  // Getters
  float getTemperature() const { return temperature; }
  float getHumidity() const { return humidity; }
  float getPressure() const { return pressure; }

  // Get pressure trend (change in hPa over 3 hours)
  // Returns: >0 rising, <0 falling, ~0 stable
  float getPressureTrend() {
    // Need at least 36 minutes of data (9 readings at 5-min intervals for 3 hours)
    int oldestIndex = (historyIndex + 1) % HISTORY_SIZE;
    if (pressureHistory[oldestIndex] == 0) {
      return 0; // Not enough data yet
    }

    float oldest = pressureHistory[oldestIndex];
    float current = pressure;
    return current - oldest; // Positive = rising, negative = falling
  }

  // Get trend arrow symbol
  const char* getTrendArrow() {
    float trend = getPressureTrend();

    if (trend > 2.0) return "↑";       // Rising rapidly
    if (trend > 0.5) return "↗";       // Rising slowly
    if (trend < -2.0) return "↓";      // Falling rapidly
    if (trend < -0.5) return "↘";      // Falling slowly
    return "→";                         // Stable
  }

  // Check if pressure is falling rapidly (storm warning)
  bool isStormApproaching() {
    return getPressureTrend() < -3.0; // Falling >3 hPa in 3 hours
  }
};

#endif // BME280_SENSOR_H
