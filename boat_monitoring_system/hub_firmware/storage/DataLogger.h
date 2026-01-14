#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <SD.h>
#include <SPI.h>
#include "../../common/CommonTypes.h"

/**
 * Data Logger - Logs sensor data and events to SD card
 */
class DataLogger {
private:
  int csPin;
  bool sdAvailable;
  String currentLogFile;

public:
  DataLogger(int cs) : csPin(cs), sdAvailable(false) {}

  bool begin() {
    if (!SD.begin(csPin)) {
      Serial.println("SD card init failed or not present");
      sdAvailable = false;
      return false;
    }

    Serial.println("SD card initialized");
    sdAvailable = true;

    // Create log file with current date (if RTC available)
    currentLogFile = "/boat_log.csv";

    // Create header if new file
    if (!SD.exists(currentLogFile)) {
      File file = SD.open(currentLogFile, FILE_WRITE);
      if (file) {
        file.println("Timestamp,NodeID,Type,Temperature,Humidity,Pressure,BatteryMv,RSSI");
        file.close();
        Serial.println("Created new log file");
      }
    }

    return true;
  }

  void logEnvironmental(uint8_t nodeID, float temp, float humidity, float pressure,
                       uint16_t batteryMv, int16_t rssi) {
    if (!sdAvailable) return;

    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
      file.print(millis() / 1000); // Timestamp in seconds
      file.print(",");
      file.print(nodeID, HEX);
      file.print(",ENV,");
      file.print(temp, 2);
      file.print(",");
      file.print(humidity, 1);
      file.print(",");
      file.print(pressure, 1);
      file.print(",");
      file.print(batteryMv);
      file.print(",");
      file.println(rssi);
      file.close();
    }
  }

  void logDetection(uint8_t nodeID, uint8_t eventType, uint8_t confidence,
                   uint16_t distance, uint8_t zone) {
    if (!sdAvailable) return;

    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
      file.print(millis() / 1000);
      file.print(",");
      file.print(nodeID, HEX);
      file.print(",DETECT,");
      file.print(eventType);
      file.print(",");
      file.print(confidence);
      file.print(",");
      file.print(distance);
      file.print(",");
      file.println(zone);
      file.close();
    }
  }

  void logAlarm(uint8_t nodeID, const char* event) {
    if (!sdAvailable) return;

    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
      file.print(millis() / 1000);
      file.print(",");
      file.print(nodeID, HEX);
      file.print(",ALARM,");
      file.println(event);
      file.close();
    }
  }

  void logEvent(const char* event) {
    if (!sdAvailable) return;

    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
      file.print(millis() / 1000);
      file.print(",HUB,EVENT,");
      file.println(event);
      file.close();
    }
  }

  bool isAvailable() const { return sdAvailable; }
};

#endif // DATA_LOGGER_H
