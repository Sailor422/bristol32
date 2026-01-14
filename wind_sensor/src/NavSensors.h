#ifndef NAV_SENSORS_H
#define NAV_SENSORS_H

#include <Adafruit_LSM303_DLH_Mag.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <TinyGPS++.h>
#include <Wire.h>

class NavSensors {
private:
  TinyGPSPlus gps;
  Adafruit_LSM303_DLH_Mag_Unified mag = Adafruit_LSM303_DLH_Mag_Unified(12345);
  HardwareSerial *gpsSerial;

public:
  void begin(HardwareSerial *serial) {
    gpsSerial = serial;
    gpsSerial->begin(9600); // Standard GPS baud

    if (!mag.begin()) {
      Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    }
  }

  void update() {
    while (gpsSerial->available() > 0) {
      gps.encode(gpsSerial->read());
    }
  }

  // Returns boat speed in mm/s
  uint16_t getBoatSpeed() {
    if (gps.speed.isValid()) {
      // knots to mm/s: 1 knot = 514.444 mm/s
      return (uint16_t)(gps.speed.knots() * 514.444);
    }
    return 0;
  }

  // Returns boat heading in degrees * 10
  // Uses Compass for Heading (Magnetic), GPS for Course Over Ground
  // For True Wind, we usually want Heading (where bow points)
  uint16_t getBoatHeading() {
    sensors_event_t event;
    mag.getEvent(&event);

    float Pi = 3.14159;

    // Calculate the angle of the vector y,x
    float heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / Pi;

    // Normalize to 0-360
    if (heading < 0) {
      heading = 360 + heading;
    }

    return (uint16_t)(heading * 10);
  }

  // Returns GPS COG in degrees * 10
  uint16_t getCourseOverGround() {
    if (gps.course.isValid()) {
      return (uint16_t)(gps.course.deg() * 10);
    }
    return 0;
  }

  uint8_t getFixQuality() {
    if (gps.location.isValid())
      return 1; // Basic valid fix
    return 0;
  }
};

#endif
