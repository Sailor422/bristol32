#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include <Arduino.h>

/**
 * Base class for all sensors
 * Provides common interface for initialization, reading, and status
 */
class SensorBase {
public:
  virtual ~SensorBase() {}

  // Initialize sensor - returns true if successful
  virtual bool begin() = 0;

  // Read sensor data - returns true if successful
  virtual bool read() = 0;

  // Check if sensor is available and responding
  virtual bool isAvailable() = 0;

  // Get human-readable status string
  virtual String getStatusString() = 0;
};

#endif // SENSOR_BASE_H
