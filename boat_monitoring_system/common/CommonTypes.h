#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Arduino.h>

// Alarm modes
enum AlarmMode {
  MODE_DISARMED = 0,
  MODE_DOORBELL = 1,
  MODE_PERIMETER = 2,
  MODE_FULL = 3,
  MODE_QUIET = 4
};

// System states
enum SystemState {
  STATE_INIT = 0,
  STATE_NORMAL = 1,
  STATE_PRE_ALARM = 2,
  STATE_ALARM_TRIGGERED = 3,
  STATE_SLEEP = 4,
  STATE_ERROR = 5
};

// Detection event structure
struct DetectionEvent {
  bool detected;
  uint8_t confidence;        // 0-100%
  uint16_t distance;         // cm
  uint8_t zone;              // 0=Near, 1=Middle, 2=Far
  unsigned long timestamp;
  unsigned long duration;    // milliseconds
  uint8_t eventType;         // 0x01=Approach, 0x02=Entry, 0x03=Doorbell
};

// Environmental data structure
struct EnvData {
  uint8_t nodeID;
  float temperature;         // Celsius
  float humidity;            // Percentage
  float pressure;            // hPa
  uint16_t batteryVoltage;   // mV
  int8_t rssi;               // dBm
};

// Node information structure
struct NodeInfo {
  uint8_t id;
  String name;
  unsigned long lastContact;
  float temperature;
  float humidity;
  float pressure;
  uint16_t batteryVoltage;
  int rssi;
  bool online;
  SystemState state;
};

// Alarm event structure
struct AlarmEvent {
  uint8_t nodeID;
  uint8_t eventType;         // 0x01=Armed, 0x02=Disarmed, 0x03=Triggered, 0x04=Silenced
  unsigned long timestamp;
  AlarmMode mode;
};

// Convert alarm mode to string
inline const char* alarmModeToString(AlarmMode mode) {
  switch (mode) {
    case MODE_DISARMED: return "DISARMED";
    case MODE_DOORBELL: return "DOORBELL";
    case MODE_PERIMETER: return "PERIMETER";
    case MODE_FULL: return "FULL ARMED";
    case MODE_QUIET: return "QUIET MODE";
    default: return "UNKNOWN";
  }
}

// Convert system state to string
inline const char* stateToString(SystemState state) {
  switch (state) {
    case STATE_INIT: return "INIT";
    case STATE_NORMAL: return "NORMAL";
    case STATE_PRE_ALARM: return "PRE-ALARM";
    case STATE_ALARM_TRIGGERED: return "ALARM!";
    case STATE_SLEEP: return "SLEEP";
    case STATE_ERROR: return "ERROR";
    default: return "UNKNOWN";
  }
}

#endif // COMMON_TYPES_H
