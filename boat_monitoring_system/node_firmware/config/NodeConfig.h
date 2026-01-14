#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

#include <Preferences.h>
#include "../../common/CommonTypes.h"

/**
 * Node Configuration Manager
 * Stores and manages node settings in non-volatile storage (NVS)
 */
class NodeConfig {
private:
  Preferences prefs;
  const char* NAMESPACE = "boat-node";

public:
  // Node identity
  uint8_t nodeID;
  uint8_t hubID;
  String nodeName;

  // Alarm settings
  AlarmMode alarmMode;
  uint8_t detectionSensitivity;    // 0-100 (affects min confidence)
  bool quietHoursEnabled;
  uint8_t quietHourStart;          // 0-23 hour
  uint8_t quietHourEnd;            // 0-23 hour

  // Detection zones (in cm)
  uint16_t nearZoneMax;
  uint16_t middleZoneMax;
  uint16_t farZoneMax;

  // Display settings
  uint8_t displayBrightness;       // 0-255
  uint16_t displayTimeout;         // seconds
  bool temperatureFahrenheit;      // true=F, false=C

  // LoRa settings
  float loraFrequency;             // MHz
  uint8_t loraTxPower;             // dBm
  uint8_t loraSpreadingFactor;     // 7-12

  // Timing
  uint32_t envDataInterval;        // milliseconds between env data transmissions
  uint32_t heartbeatInterval;      // milliseconds between heartbeats

  NodeConfig() {
    // Set defaults
    nodeID = 0x01;
    hubID = 0x00;
    nodeName = "Node 1";
    alarmMode = MODE_DISARMED;
    detectionSensitivity = 50;
    quietHoursEnabled = false;
    quietHourStart = 22;
    quietHourEnd = 6;
    nearZoneMax = 100;
    middleZoneMax = 300;
    farZoneMax = 600;
    displayBrightness = 128;
    displayTimeout = 30;
    temperatureFahrenheit = false;
    loraFrequency = 915.0;
    loraTxPower = 17;
    loraSpreadingFactor = 8;
    envDataInterval = 300000;      // 5 minutes
    heartbeatInterval = 60000;     // 1 minute
  }

  bool load() {
    prefs.begin(NAMESPACE, false);

    nodeID = prefs.getUChar("nodeID", 0x01);
    hubID = prefs.getUChar("hubID", 0x00);
    nodeName = prefs.getString("nodeName", "Node 1");
    alarmMode = (AlarmMode)prefs.getUChar("alarmMode", MODE_DISARMED);
    detectionSensitivity = prefs.getUChar("sensitivity", 50);
    quietHoursEnabled = prefs.getBool("quietHours", false);
    quietHourStart = prefs.getUChar("quietStart", 22);
    quietHourEnd = prefs.getUChar("quietEnd", 6);
    nearZoneMax = prefs.getUShort("nearZone", 100);
    middleZoneMax = prefs.getUShort("middleZone", 300);
    farZoneMax = prefs.getUShort("farZone", 600);
    displayBrightness = prefs.getUChar("brightness", 128);
    displayTimeout = prefs.getUShort("dispTimeout", 30);
    temperatureFahrenheit = prefs.getBool("tempF", false);
    loraFrequency = prefs.getFloat("loraFreq", 915.0);
    loraTxPower = prefs.getUChar("loraPower", 17);
    loraSpreadingFactor = prefs.getUChar("loraSF", 8);
    envDataInterval = prefs.getULong("envInterval", 300000);
    heartbeatInterval = prefs.getULong("hbInterval", 60000);

    prefs.end();

    Serial.println("Configuration loaded:");
    Serial.print("  Node ID: 0x");
    Serial.println(nodeID, HEX);
    Serial.print("  Node Name: ");
    Serial.println(nodeName);
    Serial.print("  Alarm Mode: ");
    Serial.println(alarmModeToString(alarmMode));

    return true;
  }

  bool save() {
    prefs.begin(NAMESPACE, false);

    prefs.putUChar("nodeID", nodeID);
    prefs.putUChar("hubID", hubID);
    prefs.putString("nodeName", nodeName);
    prefs.putUChar("alarmMode", alarmMode);
    prefs.putUChar("sensitivity", detectionSensitivity);
    prefs.putBool("quietHours", quietHoursEnabled);
    prefs.putUChar("quietStart", quietHourStart);
    prefs.putUChar("quietEnd", quietHourEnd);
    prefs.putUShort("nearZone", nearZoneMax);
    prefs.putUShort("middleZone", middleZoneMax);
    prefs.putUShort("farZone", farZoneMax);
    prefs.putUChar("brightness", displayBrightness);
    prefs.putUShort("dispTimeout", displayTimeout);
    prefs.putBool("tempF", temperatureFahrenheit);
    prefs.putFloat("loraFreq", loraFrequency);
    prefs.putUChar("loraPower", loraTxPower);
    prefs.putUChar("loraSF", loraSpreadingFactor);
    prefs.putULong("envInterval", envDataInterval);
    prefs.putULong("hbInterval", heartbeatInterval);

    prefs.end();

    Serial.println("Configuration saved");
    return true;
  }

  void factoryReset() {
    prefs.begin(NAMESPACE, false);
    prefs.clear();
    prefs.end();

    Serial.println("Factory reset complete - reloading defaults");
    load(); // Reload defaults
  }

  // Get effective alarm mode (considering quiet hours)
  AlarmMode getEffectiveAlarmMode() {
    if (quietHoursEnabled && isQuietHours()) {
      return MODE_QUIET;
    }
    return alarmMode;
  }

  // Check if currently in quiet hours
  bool isQuietHours() {
    // Note: This requires a time source (RTC or NTP)
    // For now, return false - implement when time sync is available
    // uint8_t currentHour = getCurrentHour();
    //
    // if (quietHourStart < quietHourEnd) {
    //   return currentHour >= quietHourStart && currentHour < quietHourEnd;
    // } else {
    //   return currentHour >= quietHourStart || currentHour < quietHourEnd;
    // }

    return false;
  }

  // Convert temperature based on settings
  float convertTemperature(float celsius) {
    if (temperatureFahrenheit) {
      return celsius * 9.0 / 5.0 + 32.0;
    }
    return celsius;
  }

  const char* getTemperatureUnit() {
    return temperatureFahrenheit ? "F" : "C";
  }

  // Calculate minimum confidence based on sensitivity setting
  uint8_t getMinConfidence() {
    // Map sensitivity 0-100 to confidence threshold 40-95
    // Higher sensitivity = lower confidence threshold
    return 95 - (detectionSensitivity * 55 / 100);
  }

  // Calculate minimum duration based on sensitivity
  uint16_t getMinDuration() {
    // Map sensitivity 0-100 to duration 500-3000ms
    // Higher sensitivity = shorter duration required
    return 3000 - (detectionSensitivity * 2500 / 100);
  }
};

#endif // NODE_CONFIG_H
