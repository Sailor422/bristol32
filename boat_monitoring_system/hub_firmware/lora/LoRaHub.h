#ifndef LORA_HUB_H
#define LORA_HUB_H

#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include "../../common/MessageProtocol.h"
#include "../../common/CommonTypes.h"

#define MAX_NODES 10

/**
 * LoRa Hub Communication Manager
 * Receives and processes messages from all nodes
 */
class LoRaHub {
private:
  RH_RF95 rf95;
  RHReliableDatagram* manager;
  uint8_t hubID;

  // Pin definitions
  uint8_t csPin;
  uint8_t intPin;
  uint8_t rstPin;

  // Callbacks
  typedef void (*EnvDataCallback)(uint8_t nodeID, float temp, float humidity, float pressure, uint16_t batteryMv, int8_t rssi);
  typedef void (*DetectionCallback)(uint8_t nodeID, uint8_t eventType, uint8_t confidence, uint16_t distance, uint8_t zone);
  typedef void (*AlarmCallback)(uint8_t nodeID, uint8_t command, uint8_t mode);

  EnvDataCallback onEnvData;
  DetectionCallback onDetection;
  AlarmCallback onAlarm;

public:
  LoRaHub(uint8_t cs, uint8_t interrupt, uint8_t reset)
    : rf95(cs, interrupt), csPin(cs), intPin(interrupt), rstPin(reset),
      hubID(HUB_ADDRESS), onEnvData(nullptr), onDetection(nullptr), onAlarm(nullptr) {
    manager = new RHReliableDatagram(rf95, HUB_ADDRESS);
  }

  ~LoRaHub() {
    delete manager;
  }

  bool begin(float frequency = 915.0) {
    // Reset LoRa module
    if (rstPin != 255) {
      pinMode(rstPin, OUTPUT);
      digitalWrite(rstPin, LOW);
      delay(10);
      digitalWrite(rstPin, HIGH);
      delay(10);
    }

    if (!manager->init()) {
      Serial.println("LoRa Hub init failed");
      return false;
    }

    // Configure LoRa parameters
    if (!rf95.setFrequency(frequency)) {
      Serial.println("LoRa setFrequency failed");
      return false;
    }

    rf95.setTxPower(17, false);
    rf95.setSpreadingFactor(8);
    rf95.setSignalBandwidth(125000);
    rf95.setCodingRate4(5);
    rf95.setPayloadCRC(true);

    manager->setRetries(3);
    manager->setTimeout(500);

    Serial.print("LoRa Hub initialized on ");
    Serial.print(frequency);
    Serial.println(" MHz");

    return true;
  }

  // Process incoming messages (call in loop)
  bool receive() {
    if (manager->available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      uint8_t from;

      if (manager->recvfromAck(buf, &len, &from)) {
        int16_t rssi = rf95.lastRssi();
        int8_t snr = rf95.lastSNR();

        Serial.print("Received from 0x");
        Serial.print(from, HEX);
        Serial.print(", RSSI: ");
        Serial.print(rssi);
        Serial.print(", SNR: ");
        Serial.println(snr);

        handleMessage(buf, len, from, rssi);
        return true;
      }
    }
    return false;
  }

  // Send alarm command to node(s)
  bool sendAlarmCommand(uint8_t targetNode, uint8_t command, uint8_t mode) {
    uint8_t packet[ALARM_PACKET_SIZE];
    packAlarmPacket(packet, HUB_ADDRESS, command, mode, targetNode);

    bool success = manager->sendtoWait(packet, ALARM_PACKET_SIZE, targetNode);

    if (success) {
      Serial.print("Alarm command sent to 0x");
      Serial.println(targetNode, HEX);
    } else {
      Serial.println("Alarm command failed");
    }

    return success;
  }

  // Broadcast time sync
  bool broadcastTimeSync(uint32_t timestamp) {
    uint8_t packet[10];
    packet[0] = HUB_ADDRESS;
    packet[1] = MSG_TYPE_TIME_SYNC;
    packUint32(packet, 2, timestamp);
    packet[6] = calculateChecksum(packet, 6);

    // Broadcast to all nodes
    bool success = manager->sendtoWait(packet, 7, BROADCAST_ADDRESS);

    if (success) {
      Serial.println("Time sync broadcast sent");
    }

    return success;
  }

  // Set callbacks
  void setEnvDataCallback(EnvDataCallback callback) { onEnvData = callback; }
  void setDetectionCallback(DetectionCallback callback) { onDetection = callback; }
  void setAlarmCallback(AlarmCallback callback) { onAlarm = callback; }

  int16_t getLastRSSI() { return rf95.lastRssi(); }
  int8_t getLastSNR() { return rf95.lastSNR(); }

private:
  void handleMessage(uint8_t* buf, uint8_t len, uint8_t from, int16_t rssi) {
    if (len < 2) return;

    uint8_t packetType = buf[1];

    switch (packetType) {
      case MSG_TYPE_ENVIRONMENTAL:
        handleEnvironmentalPacket(buf, len, rssi);
        break;

      case MSG_TYPE_DETECTION:
        handleDetectionPacket(buf, len);
        break;

      case MSG_TYPE_ALARM:
        handleAlarmPacket(buf, len);
        break;

      case MSG_TYPE_HEARTBEAT:
        handleHeartbeatPacket(buf, len);
        break;

      default:
        Serial.print("Unknown packet type: 0x");
        Serial.println(packetType, HEX);
        break;
    }
  }

  void handleEnvironmentalPacket(uint8_t* buf, uint8_t len, int16_t rssi) {
    if (len != ENV_PACKET_SIZE) {
      Serial.println("Invalid environmental packet size");
      return;
    }

    uint8_t nodeID;
    float temp, humidity, pressure;
    uint16_t batteryMv;
    int8_t packetRssi;

    if (unpackEnvironmentalPacket(buf, &nodeID, &temp, &humidity, &pressure, &batteryMv, &packetRssi)) {
      Serial.print("Environmental data from 0x");
      Serial.print(nodeID, HEX);
      Serial.print(": ");
      Serial.print(temp, 1);
      Serial.print("C, ");
      Serial.print((int)humidity);
      Serial.print("%, ");
      Serial.print((int)pressure);
      Serial.println("hPa");

      if (onEnvData) {
        onEnvData(nodeID, temp, humidity, pressure, batteryMv, rssi);
      }
    } else {
      Serial.println("Environmental packet checksum failed");
    }
  }

  void handleDetectionPacket(uint8_t* buf, uint8_t len) {
    if (len != DETECTION_PACKET_SIZE) {
      Serial.println("Invalid detection packet size");
      return;
    }

    uint8_t nodeID, eventType, confidence, zone;
    uint16_t distance;

    if (unpackDetectionPacket(buf, &nodeID, &eventType, &confidence, &distance, &zone)) {
      Serial.print("Detection from 0x");
      Serial.print(nodeID, HEX);
      Serial.print(": Type=");
      Serial.print(eventType);
      Serial.print(", Conf=");
      Serial.print(confidence);
      Serial.print("%, Dist=");
      Serial.print(distance);
      Serial.println("cm");

      if (onDetection) {
        onDetection(nodeID, eventType, confidence, distance, zone);
      }
    }
  }

  void handleAlarmPacket(uint8_t* buf, uint8_t len) {
    if (len != ALARM_PACKET_SIZE) return;

    uint8_t nodeID, command, mode, target;
    if (unpackAlarmPacket(buf, &nodeID, &command, &mode, &target)) {
      Serial.print("Alarm from 0x");
      Serial.print(nodeID, HEX);
      Serial.print(": Cmd=");
      Serial.println(command);

      if (onAlarm) {
        onAlarm(nodeID, command, mode);
      }
    }
  }

  void handleHeartbeatPacket(uint8_t* buf, uint8_t len) {
    if (len != HEARTBEAT_PACKET_SIZE) return;

    uint8_t nodeID;
    uint16_t batteryMv;

    if (unpackHeartbeatPacket(buf, &nodeID, &batteryMv)) {
      Serial.print("Heartbeat from 0x");
      Serial.print(nodeID, HEX);
      Serial.print(": Battery=");
      Serial.print(batteryMv);
      Serial.println("mV");
    }
  }
};

#endif // LORA_HUB_H
