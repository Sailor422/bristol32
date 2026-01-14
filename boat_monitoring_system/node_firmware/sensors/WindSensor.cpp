#include "WindSensor.h"

// Wind direction lookup tables
const int WIND_DIR_RESISTOR_VALUES[] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000};
const float WIND_DIR_ANGLES[] = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5};
const int WIND_DIR_TABLE_SIZE = 16;

WindSensor::WindSensor() {
    apparentWindSpeed = 0.0;
    apparentWindDirection = 0.0;
    trueWindSpeed = 0.0;
    trueWindDirection = 0.0;
    boatSpeed = 0.0;
    boatHeading = 0.0;
    boatCourse = 0.0;
}

void WindSensor::begin() {
    // Initialize GPS
    gps = new Adafruit_GPS(&Serial2);
    gps->begin(9600);
    gps->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

    // Initialize compass
    accel = new Adafruit_LSM303_Accel_Unified(30301);
    mag = new Adafruit_LSM303DLH_Mag_Unified(30302);

    if (!accel->begin()) {
        Serial.println("LSM303 accelerometer not found");
    }

    if (!mag->begin()) {
        Serial.println("LSM303 magnetometer not found");
    }

    // Configure wind sensor pins
    pinMode(WIND_SPEED_PIN, INPUT);
    pinMode(WIND_DIR_PIN, INPUT);
}

void WindSensor::update() {
    // Read GPS data
    gps->read();
    if (gps->newNMEAreceived()) {
        if (gps->parse(gps->lastNMEA())) {
            boatSpeed = gps->speed * 0.514444; // Convert knots to m/s
            boatCourse = gps->angle;
        }
    }

    // Read compass data
    sensors_event_t accel_event;
    sensors_event_t mag_event;
    accel->getEvent(&accel_event);
    mag->getEvent(&mag_event);

    // Calculate heading from magnetometer
    float heading = atan2(mag_event.magnetic.y, mag_event.magnetic.x) * 180.0 / PI;
    boatHeading = normalizeAngle(heading);

    // Read wind sensors
    apparentWindSpeed = readWindSpeed();
    apparentWindDirection = readWindDirection();

    // Calculate true wind
    calculateTrueWind();
}

float WindSensor::readWindSpeed() {
    int adcValue = analogRead(WIND_SPEED_PIN);
    float voltage = adcValue * (3.3 / 4095.0); // ESP32 ADC

    if (voltage < WIND_SPEED_V_MIN) return 0.0;

    // Linear interpolation between min and max
    float speed = (voltage - WIND_SPEED_V_MIN) / (WIND_SPEED_V_MAX - WIND_SPEED_V_MIN) * WIND_SPEED_MAX;
    return constrain(speed, 0.0, WIND_SPEED_MAX);
}

float WindSensor::readWindDirection() {
    int adcValue = analogRead(WIND_DIR_PIN);
    float voltage = adcValue * (3.3 / 4095.0);
    int resistance = voltage / (3.3 - voltage) * 10000; // Assuming 10k pull-up

    // Find closest resistance value in lookup table
    int closestIndex = 0;
    int minDiff = abs(resistance - WIND_DIR_RESISTOR_VALUES[0]);

    for (int i = 1; i < WIND_DIR_TABLE_SIZE; i++) {
        int diff = abs(resistance - WIND_DIR_RESISTOR_VALUES[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = i;
        }
    }

    return WIND_DIR_ANGLES[closestIndex];
}

void WindSensor::calculateTrueWind() {
    // Convert angles to radians for calculations
    float apparentAngleRad = apparentWindDirection * PI / 180.0;
    float boatHeadingRad = boatHeading * PI / 180.0;

    // Vector components
    float apparentX = apparentWindSpeed * cos(apparentAngleRad);
    float apparentY = apparentWindSpeed * sin(apparentAngleRad);

    float boatX = boatSpeed * cos(boatHeadingRad);
    float boatY = boatSpeed * sin(boatHeadingRad);

    // True wind vector components
    float trueX = apparentX - boatX;
    float trueY = apparentY - boatY;

    // True wind speed and direction
    trueWindSpeed = sqrt(trueX * trueX + trueY * trueY);
    trueWindDirection = atan2(trueY, trueX) * 180.0 / PI;
    trueWindDirection = normalizeAngle(trueWindDirection);
}

float WindSensor::normalizeAngle(float angle) {
    while (angle < 0) angle += 360.0;
    while (angle >= 360.0) angle -= 360.0;
    return angle;
}