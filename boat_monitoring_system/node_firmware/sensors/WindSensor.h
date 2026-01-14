#ifndef WIND_SENSOR_H
#define WIND_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_Sensor.h>

// Wind sensor pins
#define WIND_SPEED_PIN A0        // Analog input for anemometer
#define WIND_DIR_PIN A1          // Analog input for wind vane

// Calibration constants
#define WIND_SPEED_V_MIN 0.4     // Minimum voltage for wind speed
#define WIND_SPEED_V_MAX 2.0     // Maximum voltage for wind speed
#define WIND_SPEED_MAX 32.4      // Maximum wind speed in m/s

// Wind direction potentiometer values (0-360 degrees)
#define WIND_DIR_RESISTOR_VALUES {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000}
#define WIND_DIR_ANGLES {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5}

class WindSensor {
public:
    WindSensor();
    void begin();
    void update();

    // Wind data
    float apparentWindSpeed;    // m/s
    float apparentWindDirection; // degrees (0-360)
    float trueWindSpeed;        // m/s
    float trueWindDirection;    // degrees (0-360)

    // Boat motion data
    float boatSpeed;            // m/s (from GPS)
    float boatHeading;          // degrees (from compass)
    float boatCourse;           // degrees (from GPS)

private:
    // Sensor objects
    Adafruit_GPS* gps;
    Adafruit_LSM303_Accel_Unified* accel;
    Adafruit_LSM303DLH_Mag_Unified* mag;

    // Helper functions
    float readWindSpeed();
    float readWindDirection();
    void calculateTrueWind();
    float normalizeAngle(float angle);
};

#endif // WIND_SENSOR_H