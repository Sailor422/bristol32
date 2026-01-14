#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include "../../common/CommonTypes.h"

/**
 * Alarm State Manager for Hub
 * Coordinates alarm state across all nodes
 */
class AlarmManager {
private:
  AlarmMode currentMode;
  bool alarmTriggered;
  uint8_t triggeringNode;
  unsigned long alarmTime;
  String alarmPin;

public:
  AlarmManager() : currentMode(MODE_DISARMED), alarmTriggered(false),
                  triggeringNode(0), alarmTime(0), alarmPin("1234") {}

  void begin() {
    currentMode = MODE_DISARMED;
    alarmTriggered = false;
    Serial.println("Alarm Manager initialized");
  }

  void armSystem(AlarmMode mode) {
    if (mode != MODE_DISARMED) {
      Serial.print("Arming system: ");
      Serial.println(alarmModeToString(mode));
    }
    currentMode = mode;
    alarmTriggered = false;
  }

  void disarm() {
    Serial.println("System disarmed");
    currentMode = MODE_DISARMED;
    alarmTriggered = false;
    triggeringNode = 0;
  }

  void triggerAlarm(uint8_t nodeID) {
    if (currentMode == MODE_DISARMED) return; // Can't trigger when disarmed

    if (!alarmTriggered) {
      Serial.print("ALARM TRIGGERED by node 0x");
      Serial.println(nodeID, HEX);
      alarmTriggered = true;
      triggeringNode = nodeID;
      alarmTime = millis();
    }
  }

  bool verifyPin(const String& pin) {
    return pin.equals(alarmPin);
  }

  void setPin(const String& newPin) {
    alarmPin = newPin;
  }

  // Getters
  AlarmMode getMode() const { return currentMode; }
  bool isTriggered() const { return alarmTriggered; }
  uint8_t getTriggeringNode() const { return triggeringNode; }
  unsigned long getAlarmDuration() const {
    return alarmTriggered ? (millis() - alarmTime) / 1000 : 0;
  }

  void process() {
    // Auto-timeout after 10 minutes (optional safety feature)
    if (alarmTriggered && (millis() - alarmTime) > 600000) {
      Serial.println("Alarm auto-timeout after 10 minutes");
      disarm();
    }
  }
};

#endif // ALARM_MANAGER_H
