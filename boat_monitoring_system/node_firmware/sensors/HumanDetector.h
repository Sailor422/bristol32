#ifndef HUMAN_DETECTOR_H
#define HUMAN_DETECTOR_H

#include "SensorBase.h"
#include "../../common/CommonTypes.h"

/**
 * Human Presence Detector using mmWave Radar
 * Supports HLK-LD2410 and similar UART-based mmWave sensors
 */
class HumanDetector : public SensorBase {
private:
  HardwareSerial* serial;
  int rxPin;
  int txPin;

  bool presenceDetected;
  uint16_t distance;         // cm
  uint8_t confidence;        // 0-100%
  uint8_t zone;              // 0=Near, 1=Middle, 2=Far
  unsigned long detectionStartTime;
  unsigned long lastReadTime;

  // Configuration
  uint16_t nearZoneMax;      // cm (default 100)
  uint16_t middleZoneMax;    // cm (default 300)
  uint16_t farZoneMax;       // cm (default 600)
  uint8_t minConfidence;     // Minimum confidence to trigger (default 70)
  uint16_t minDuration;      // Minimum detection duration in ms (default 2000)

  // IMU integration (optional, for boat motion compensation)
  bool useMotionCompensation;
  float motionThreshold;

public:
  HumanDetector(int rx, int tx, HardwareSerial* ser = &Serial2)
    : serial(ser), rxPin(rx), txPin(tx),
      presenceDetected(false), distance(0), confidence(0), zone(0),
      detectionStartTime(0), lastReadTime(0),
      nearZoneMax(100), middleZoneMax(300), farZoneMax(600),
      minConfidence(70), minDuration(2000),
      useMotionCompensation(false), motionThreshold(5.0) {}

  bool begin() override {
    serial->begin(115200, SERIAL_8N1, rxPin, txPin);
    delay(100);

    // Send configuration to HLK-LD2410 (if supported)
    // This is sensor-specific and may need adjustment
    configureLD2410();

    lastReadTime = millis();
    return true;
  }

  bool read() override {
    // Parse incoming UART data from mmWave sensor
    // This is a simplified implementation - actual parsing depends on sensor protocol

    if (serial->available()) {
      // For HLK-LD2410, data format is proprietary
      // This is a placeholder - implement actual protocol parsing
      uint8_t buf[32];
      int len = serial->readBytes(buf, min(serial->available(), 32));

      // Simplified parsing (replace with actual protocol)
      if (len >= 8) {
        presenceDetected = buf[0] & 0x01;
        distance = (buf[2] << 8) | buf[3];
        confidence = buf[4];

        // Determine zone based on distance
        if (distance <= nearZoneMax) {
          zone = 0; // Near
        } else if (distance <= middleZoneMax) {
          zone = 1; // Middle
        } else if (distance <= farZoneMax) {
          zone = 2; // Far
        } else {
          presenceDetected = false; // Too far, ignore
        }

        lastReadTime = millis();
      }
    }

    return true;
  }

  bool isAvailable() override {
    return serial && (millis() - lastReadTime < 5000); // Sensor responding within 5s
  }

  String getStatusString() override {
    if (presenceDetected) {
      char buf[64];
      snprintf(buf, sizeof(buf), "DETECTED %dcm (%d%% conf)",
               distance, confidence);
      return String(buf);
    }
    return "Clear";
  }

  // Check if there's a valid detection event
  bool detectionEvent() {
    if (!presenceDetected) {
      detectionStartTime = 0;
      return false;
    }

    // First detection
    if (detectionStartTime == 0) {
      detectionStartTime = millis();
      return false; // Not sustained yet
    }

    // Check sustained duration
    unsigned long duration = millis() - detectionStartTime;
    if (duration < minDuration) {
      return false; // Not sustained long enough
    }

    // Check confidence threshold
    if (confidence < minConfidence) {
      detectionStartTime = 0;
      return false;
    }

    return true; // Valid detection
  }

  // Get detection event structure
  DetectionEvent getEvent() {
    DetectionEvent event;
    event.detected = presenceDetected;
    event.confidence = confidence;
    event.distance = distance;
    event.zone = zone;
    event.timestamp = millis();
    event.duration = detectionStartTime > 0 ? (millis() - detectionStartTime) : 0;
    event.eventType = 0x01; // Approach (default)

    // Determine event type based on zone
    if (zone == 0) {
      event.eventType = 0x02; // Entry (near zone)
    } else if (zone >= 1) {
      event.eventType = 0x01; // Approach (middle/far zone)
    }

    return event;
  }

  // Configuration methods
  void setZones(uint16_t near, uint16_t middle, uint16_t far) {
    nearZoneMax = near;
    middleZoneMax = middle;
    farZoneMax = far;
  }

  void setSensitivity(uint8_t minConf, uint16_t minDur) {
    minConfidence = minConf;
    minDuration = minDur;
  }

  void enableMotionCompensation(bool enable, float threshold = 5.0) {
    useMotionCompensation = enable;
    motionThreshold = threshold;
  }

  // Getters
  bool isDetected() const { return presenceDetected; }
  uint16_t getDistance() const { return distance; }
  uint8_t getConfidence() const { return confidence; }
  uint8_t getZone() const { return zone; }

private:
  void configureLD2410() {
    // Send configuration commands to HLK-LD2410
    // This is sensor-specific and should be adjusted based on datasheet

    // Example: Enable engineering mode (if needed)
    // uint8_t enableCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
    // serial->write(enableCmd, sizeof(enableCmd));
    // delay(50);

    // Set max detection range (example: 6 meters)
    // uint8_t rangeCmd[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x14, 0x00, 0x60, 0x00, ...};
    // serial->write(rangeCmd, sizeof(rangeCmd));
    // delay(50);

    // Note: Implement actual configuration based on sensor documentation
    Serial.println("HumanDetector: Configure sensor (implement sensor-specific commands)");
  }
};

#endif // HUMAN_DETECTOR_H
